//#include "FullscreenShaderStructs.hlsli"
#include "BoxLightShaderStructs.hlsli"
#include "MathHelpers.hlsli"

#define NUM_SAMPLES 128
#define NUM_SAMPLES_RCP 0.0078125

// RAYMARCHING
#define TAU 0.0001
#define PHI /*10000000.0*/100000.0

#define PI_RCP 0.31830988618379067153776752674503

cbuffer FrameBuffer : register(b0)
{
    float4x4 toCameraSpace;
    float4x4 toWorldFromCamera;
    float4x4 toProjectionSpace;
    float4x4 toCameraFromProjection;
    float4 cameraPosition;
}

sampler defaultSampler : register(s0);
sampler shadowSampler : register(s1);
Texture2D depthTexture : register(t21);
Texture2D shadowDepthTexture : register(t22);

bool RayPlaneIntersection(float3 planeCenter, float3 planeNormal, float3 rayOrigin, float3 rayDirection, inout float t)
{
    float denom = dot(planeNormal, rayDirection);
    if (/*abs*/(denom) > 0.0001f)
    {
        t = dot((planeCenter - rayOrigin), planeNormal) / denom;
        if (t > 0.0f)
            return true;
    }
    return false;
}

float3 FindRayIntersection(float3 rayOrigin, float3 rayDirection)
{
    float3 intersection = float3(0.0f, 0.0f, 0.0f);
    float t = 0.0f;
    float tClosestIntersection = 1000000.0f;
    float3 halfRight = boxLightDirectionNormal1.xyz * 0.5f * boxLightWidthAndHeight.x;
    float3 halfUp = boxLightDirectionNormal2.xyz * 0.5f * boxLightWidthAndHeight.y;
    
    float3 backCenter = boxLightPositionAndRange.xyz;
    float3 frontCenter = boxLightPositionAndRange.xyz + boxLightDirection.xyz * boxLightPositionAndRange.w;
    float3 upCenter = (boxLightPositionAndRange.xyz + boxLightDirection.xyz * 0.5f * boxLightPositionAndRange.w);
    float3 downCenter = upCenter;
    float3 leftCenter = upCenter;
    float3 rightCenter = upCenter;
    
    upCenter += halfUp;
    downCenter -= halfUp;
    rightCenter += halfRight;
    leftCenter -= halfRight;

    // Back
    if (RayPlaneIntersection(backCenter, -boxLightDirection.xyz, rayOrigin, rayDirection, t))
    {
        if (t < tClosestIntersection)
        {
            tClosestIntersection = t;
            intersection = rayOrigin + rayDirection * tClosestIntersection;
        }
    }
    // Front
    if (RayPlaneIntersection(frontCenter, boxLightDirection.xyz, rayOrigin, rayDirection, t))
    {
        if (t < tClosestIntersection)
        {
            tClosestIntersection = t;
            intersection = rayOrigin + rayDirection * tClosestIntersection;
        }
    }
    // Up
    if (RayPlaneIntersection(upCenter, boxLightDirectionNormal2.xyz, rayOrigin, rayDirection, t))
    {
        if (t < tClosestIntersection)
        {
            tClosestIntersection = t;
            intersection = rayOrigin + rayDirection * tClosestIntersection;
        }
    }
    // Down
    if (RayPlaneIntersection(downCenter, -boxLightDirectionNormal2.xyz, rayOrigin, rayDirection, t))
    {
        if (t < tClosestIntersection)
        {
            tClosestIntersection = t;
            intersection = rayOrigin + rayDirection * tClosestIntersection;
        }
    }
    // Right
    if (RayPlaneIntersection(rightCenter, boxLightDirectionNormal1.xyz, rayOrigin, rayDirection, t))
    {
        if (t < tClosestIntersection)
        {
            tClosestIntersection = t;
            intersection = rayOrigin + rayDirection * tClosestIntersection;
        }
    }
    // Left
    if (RayPlaneIntersection(leftCenter, -boxLightDirectionNormal1.xyz, rayOrigin, rayDirection, t))
    {
        if (t < tClosestIntersection)
        {
            tClosestIntersection = t;
            intersection = rayOrigin + rayDirection * tClosestIntersection;
        }
    }
    
    return intersection;
}

float4 PixelShader_WorldPosition(float2 uv)
{
    // Depth sampling
    float z = depthTexture.Sample(defaultSampler, uv).r;
    float x = uv.x * 2.0f - 1;
    float y = (1 - uv.y) * 2.0f - 1;
    float4 projectedPos = float4(x, y, z, 1.0f);
    float4 viewSpacePos = mul(toCameraFromProjection, projectedPos);
    viewSpacePos /= viewSpacePos.w;
    float4 worldPos = mul(toWorldFromCamera, viewSpacePos);

    worldPos.a = 1.0f;
    return worldPos;
}

float3 SampleShadowPos(float3 projectionPos)
{
    float2 uvCoords = projectionPos.xy;
    uvCoords *= float2(0.5f, -0.5f);
    uvCoords += float2(0.5f, 0.5f);

    float nonLinearDepth = shadowDepthTexture.Sample(shadowSampler, uvCoords).r;
    float oob = 1.0f;
    if (projectionPos.x > 1.0f || projectionPos.x < -1.0f || projectionPos.y > 1.0f || projectionPos.y < -1.0f)
    {
        oob = 0.0f;
    }

    float a = nonLinearDepth * oob;
    float b = projectionPos.z;
    b = invLerp(-0.5f, 0.5f, b) * oob;

    b *= oob;

    //return 1.0f;
    
    if (b - a < 0.001f)
    {
        return 0.0f;
    }
    else
    {
        return 1.0f;
    }
}

float3 ShadowFactor(float3 viewPos)
{
    float4 projectionPos = mul(toBoxLightProjection, viewPos);
    return SampleShadowPos(projectionPos.xyz);
}

void ExecuteRaymarching(inout float3 rayPositionLightVS, float3 invViewDirLightVS, inout float3 rayPositionWorld, float3 invViewDirWorld, float stepSize, float l, inout float3 VLI)
{
    rayPositionLightVS.xyz += stepSize * invViewDirLightVS.xyz;

    // March in world space in parallel
    rayPositionWorld += stepSize * invViewDirWorld;
    //..
    
    //float3 shadowTerm = ShadowFactor(rayPositionLightVS.xyz).xxx;
    float3 shadowTerm = 1.0f;
    
    // Distance to the current position on the ray in light view-space
    float d = length(rayPositionLightVS.xyz);
    float dRcp = rcp(d); // reciprocal
    
    // Calculate the final light contribution for the sample on the ray...
    float3 intens = TAU * (shadowTerm * (PHI * 0.25 * PI_RCP) * dRcp * dRcp) * exp(-d * TAU) * exp(-l * TAU) * stepSize;

    // World space based attenuation
    float3 lightToPixel = rayPositionWorld.xyz - boxLightPositionAndRange.xyz;
    float3 projectedOnDirection = dot(lightToPixel, boxLightDirection.xyz) * boxLightDirection.xyz;
    float projectedDistanceAlongDirection = length(projectedOnDirection);
    float directionalAttenuation = projectedDistanceAlongDirection / boxLightPositionAndRange.w;
    directionalAttenuation = saturate(1.0f - directionalAttenuation);
    float physicalProjectedAttenuation = saturate(1.0f / (projectedDistanceAlongDirection * projectedDistanceAlongDirection));
   
    // ... and add it to the total contribution of the ray
    VLI += intens * directionalAttenuation * physicalProjectedAttenuation;
}

// !RAYMARCHING

BoxLightPixelOutput main(BoxLightVertexToPixel input)
{
    BoxLightPixelOutput output;
    
    // ...
    float3 worldPosition = input.myWorldPosition;
    float3 camPos = cameraPosition.xyz;
    
    float3 rayPositionWorld = worldPosition;
    float3 invViewDirWorld = normalize(camPos - worldPosition);
    
    worldPosition -= boxLightPositionAndRange.xyz;
    float3 positionLightVS = mul(toBoxLightView, worldPosition);
   
    camPos -= boxLightPositionAndRange.xyz;
    float3 cameraPositionLightVS = mul(toBoxLightView, camPos);
    
    float3 intersection = FindRayIntersection(rayPositionWorld, invViewDirWorld);
    float raymarchDistanceLimit = length(intersection - rayPositionWorld);
    
    // Reduce noisyness by truncating the starting position
    //float raymarchDistance = trunc(clamp(length(cameraPositionLightVS.xyz - positionLightVS.xyz), 0.0f, raymarchDistanceLimit));
    float4 invViewDirLightVS = float4(normalize(cameraPositionLightVS.xyz - positionLightVS.xyz), 0.0f);
    //float raymarchDistance = clamp(length(cameraPositionLightVS.xyz - positionLightVS.xyz), 0.0f, raymarchDistanceLimit);
    float raymarchDistance = raymarchDistanceLimit;
    
    // Calculate the size of each step
    float stepSize = raymarchDistance * NUM_SAMPLES_RCP;
    float3 rayPositionLightVS = positionLightVS.xyz;
    
    // The total light contribution accumulated along the ray
    float3 VLI = 0.0f;
    
    // Start ray marching
    [loop]
    for (float l = raymarchDistance; l > stepSize; l -= stepSize)
    {
        ExecuteRaymarching(rayPositionLightVS, invViewDirLightVS.xyz, rayPositionWorld, invViewDirWorld, stepSize, l, VLI);
    }
    
    output.myColor.rgb = boxLightColorAndIntensity.rgb * VLI;
    output.myColor.a = 1.0f;
    //output.myColor.rgb = 1 - exp(-raymarchDistanceLimit);
    return output;
}