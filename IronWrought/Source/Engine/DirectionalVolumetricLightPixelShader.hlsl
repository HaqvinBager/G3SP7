#include "FullscreenShaderStructs.hlsli"
#include "MathHelpers.hlsli"

#define NUM_SAMPLES 32
#define NUM_SAMPLES_RCP 0.03125

// RAYMARCHING
#define TAU 0.0001
#define PHI /*10000000.0*/5000000.0

#define PI_RCP 0.31830988618379067153776752674503

#define INTERLEAVED_GRID_SIZE 8
#define INTERLEAVED_GRID_SIZE_SQR 64
#define INTERLEAVED_GRID_SIZE_SQR_RCP 0.015625
#define USE_RANDOM_RAY_SAMPLES

cbuffer FrameBuffer : register(b0)
{
    float4x4 toCameraSpace;
    float4x4 toWorldFromCamera;
    float4x4 toProjectionSpace;
    float4x4 toCameraFromProjection;
    float4 cameraPosition;
}

cbuffer LightBuffer : register(b1)
{
    float4x4 toDirectionalLightView;
    float4x4 toDirectionalLightProjection;
    float4 directionalLightPosition; // For shadow calculations
    float4 toDirectionalLight;
    float4 directionalLightColor;
}

sampler shadowSampler : register(s1);
Texture2D depthTexture : register(t21);
Texture2D shadowDepthTexture : register(t22);

const float4 lightVolumetricRandomRaySamples[16] = { float4(34, 6, 22, 62), float4(25, 15, 10, 56), float4(29, 18, 46, 35), float4(5, 9, 37, 24), float4(60, 13, 19, 28), float4(17, 41, 11, 31), float4(58, 51, 40, 59), float4(7, 27, 50, 21), float4(4, 48, 53, 45), float4(43, 39, 20, 42), float4(44, 49, 38, 54), float4(23, 2, 33, 47), float4(1, 26, 61, 3), float4(12, 57, 36, 30), float4(55, 16, 0, 52), float4(14, 32, 8, 63) };

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
        return 1.0f;
    }
    else
    {
        return 0.0f;
    }
}

float3 ShadowFactor(float3 viewPos)
{
    float4 projectionPos = mul(toDirectionalLightProjection, viewPos);
    return SampleShadowPos(projectionPos.xyz);
}

void ExecuteRaymarching(inout float3 rayPositionLightVS, float3 invViewDirLightVS, float stepSize, float l, inout float3 VLI)
{
    rayPositionLightVS.xyz += stepSize * invViewDirLightVS.xyz;

    float3 shadowTerm = ShadowFactor(rayPositionLightVS.xyz).xxx;
    
    // Distance to the current position on the ray in light view-space
    float d = length(rayPositionLightVS.xyz);
    float dRcp = rcp(d); // reciprocal
    
    // Calculate the final light contribution for the sample on the ray...
    float3 intens = TAU * (shadowTerm * (PHI * 0.25 * PI_RCP) * dRcp * dRcp) * exp(-d * TAU) * exp(-l * TAU) * stepSize;
    
    // ... and add it to the total contribution of the ray
    VLI += intens;
}

// !RAYMARCHING

PixelOutput main(VertexToPixel input)
{
    PixelOutput output;
    
    float raymarchDistanceLimit = 99999.0f;
    
    // ...
    float3 worldPosition = PixelShader_WorldPosition(input.myUV).rgb;
    float3 camPos = cameraPosition.xyz;
    
    worldPosition -= directionalLightPosition.xyz;
    float3 positionLightVS = mul(toDirectionalLightView, worldPosition);
   
    camPos -= directionalLightPosition.xyz;
    float3 cameraPositionLightVS = mul(toDirectionalLightView, camPos);
    
    // Reduce noisyness by truncating the starting position
    //float raymarchDistance = trunc(clamp(length(cameraPositionLightVS.xyz - positionLightVS.xyz), 0.0f, raymarchDistanceLimit));
    float4 invViewDirLightVS = float4(normalize(cameraPositionLightVS.xyz - positionLightVS.xyz), 0.0f);
    float raymarchDistance = clamp(length(cameraPositionLightVS.xyz - positionLightVS.xyz), 0.0f, raymarchDistanceLimit);
    
    // Calculate the size of each step
    float stepSize = raymarchDistance * NUM_SAMPLES_RCP;

    // Calculate the offsets on the ray according to the interleaved sampling pattern
    float2 interleavedPos = fmod(worldPosition.xy, INTERLEAVED_GRID_SIZE);
#if defined(USE_RANDOM_RAY_SAMPLES)
    float index = (interleavedPos.y * INTERLEAVED_GRID_SIZE + interleavedPos.x);
    // lightVolumetricRandomRaySamples contains the values 0..63 in a randomized order
    // The indices are packed to float4s => { (0,1,2,3), (4,5,6,7), ... }
    float rayStartOffset = lightVolumetricRandomRaySamples[index * 0.25][fmod(index, 4.0f)] * (stepSize * INTERLEAVED_GRID_SIZE_SQR_RCP);
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
        ExecuteRaymarching(rayPositionLightVS, invViewDirLightVS.xyz, stepSize, l, VLI);
    }
    
    output.myColor.rgb = directionalLightColor.rgb * VLI;
    output.myColor.a = 1.0f;
    return output;
}