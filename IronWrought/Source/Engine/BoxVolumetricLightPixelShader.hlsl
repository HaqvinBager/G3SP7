#include "BoxLightShaderStructs.hlsli"
#include "VolumetricLightShaderStructs.hlsli"
#include "MathHelpers.hlsli"

sampler defaultSampler : register(s0);

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
    
    // World space based attenuation
    float3 lightToPixel = rayPositionWorld.xyz - boxLightPositionAndRange.xyz;
    float3 projectedOnDirection = dot(lightToPixel, boxLightDirection.xyz) * boxLightDirection.xyz;
    float projectedDistanceAlongDirection = length(projectedOnDirection);
    float directionalAttenuation = projectedDistanceAlongDirection / boxLightPositionAndRange.w;
    directionalAttenuation = saturate(1.0f - directionalAttenuation);
    float physicalProjectedAttenuation = saturate(1.0f / (projectedDistanceAlongDirection * projectedDistanceAlongDirection));
   
    // Distance to the current position on the ray in light view-space
    //float d = length(rayPositionLightVS.xyz);
    //float dRcp = rcp(d); // reciprocal
    float d = projectedDistanceAlongDirection; // World space distance from plane
    float dRcp = rcp(d);
    
    // Calculate the final light contribution for the sample on the ray...
    //float phase = 0.25f * PI_RCP; // isotropic scattering
    float3 toRayFromLight = normalize(rayPositionWorld.xyz - boxLightPositionAndRange.xyz);
    float projection = dot(toRayFromLight, boxLightDirection.xyz);
    float phase = PhaseFunctionHenyeyGreenstein(projection, henyeyGreensteinGValue);
    //float phase = PhaseFunctionMieHazy(projection);
    //float phase = PhaseFunctionMieMurky(projection);
    
    float3 intens = scatteringProbability * (shadowTerm * (lightPower * phase) * dRcp * dRcp) * exp(-d * scatteringProbability) * exp(-l * scatteringProbability) * stepSize;
    
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
    float stepSize = raymarchDistance * numberOfSamplesReciprocal;
    const float lightVolumetricRandomRaySampleslocal[64] = { 34.0f, 6, 22, 62, 25, 15, 10, 56, 29, 18, 46, 35, 5, 9, 37, 24, 60, 13, 19, 28, 17, 41, 11, 31, 58, 51, 40, 59, 7, 27, 50, 21, 4, 48, 53, 45, 43, 39, 20, 42, 44, 49, 38, 54, 23, 2, 33, 47, 1, 26, 61, 3, 12, 57, 36, 30, 55, 16, 0, 52, 14, 32, 8, 63 };
    
 // Calculate the offsets on the ray according to the interleaved sampling pattern
    float2 interleavedPos = fmod(worldPosition.xy, INTERLEAVED_GRID_SIZE);
#if defined(USE_RANDOM_RAY_SAMPLES)
    float index = (interleavedPos.y * INTERLEAVED_GRID_SIZE + interleavedPos.x);
    // lightVolumetricRandomRaySamples contains the values 0..63 in a randomized order
    // The indices are packed to float4s => { (0,1,2,3), (4,5,6,7), ... }
    float rayStartOffset = lightVolumetricRandomRaySampleslocal[index /* * 0.25][fmod(index, 4.0f)*/] * (stepSize * INTERLEAVED_GRID_SIZE_SQR_RCP);
#else
    float rayStartOffset = (interleavedPos.y * INTERLEAVED_GRID_SIZE + interleavedPos.x) * (stepSize * INTERLEAVED_GRID_SIZE_SQR_RCP);
#endif // USE_RANDOM_RAY_SAMPLES
    
    //float3 rayPositionLightVS = positionLightVS.xyz;
    float3 rayPositionLightVS = rayStartOffset * invViewDirLightVS.xyz + positionLightVS.xyz;
    
    // The total light contribution accumulated along the ray
    float3 VLI = 0.0f;
    
    // Start ray marching
    [loop]
    for (float l = raymarchDistance; l > 2.0f * stepSize; l -= stepSize)
    {
        ExecuteRaymarching(rayPositionLightVS, invViewDirLightVS.xyz, rayPositionWorld, invViewDirWorld, stepSize, l, VLI);
    }
    
    output.myColor.rgb = boxLightColorAndIntensity.rgb * VLI;
    output.myColor.a = 1.0f;
    //output.myColor.rgb = 1 - exp(-raymarchDistanceLimit);
    return output;
}