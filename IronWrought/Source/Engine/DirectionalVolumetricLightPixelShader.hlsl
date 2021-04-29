#include "FullscreenShaderStructs.hlsli"
#include "VolumetricLightShaderStructs.hlsli"
#include "MathHelpers.hlsli"

cbuffer LightBuffer : register(b1)
{
    float4x4 toDirectionalLightView;
    float4x4 toDirectionalLightProjection;
    float4 directionalLightPosition; // For shadow calculations
    float4 toDirectionalLight;
    float4 directionalLightColor;
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

    if (abs(uvCoords.x) > 1.0f || abs(uvCoords.y) > 1.0f)
        return 0.0f;
    
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
    float phase = 0.25f * PI_RCP;
    //float projection = dot(invViewDirLightVS, -toDirectionalLight.xyz);
    //phase = PhaseFunctionHenyeyGreenstein(projection, henyeyGreensteinGValue);
    float3 intens = scatteringProbability * (shadowTerm * (lightPower * phase) * dRcp * dRcp) * exp(-d * scatteringProbability) * exp(-l * scatteringProbability) * stepSize;
    
    // ... and add it to the total contribution of the ray
    VLI += intens;
}

// !RAYMARCHING

PixelOutput main(VertexToPixel input)
{
    PixelOutput output;
    
    float raymarchDistanceLimit = 99999.0f;
    
    //float z = depthTexture.Sample(defaultSampler, input.myUV).r;
    //if (z > 0.9999f)
    //{
    //    discard;
    //}
    
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
    float raymarchDistance = /*trunc(*/clamp(length(cameraPositionLightVS.xyz - positionLightVS.xyz), 0.0f, raymarchDistanceLimit)/*)*/;
    
    // Calculate the size of each step
    float stepSize = raymarchDistance * numberOfSamplesReciprocal;
    
    // Calculate the offsets on the ray according to the interleaved sampling pattern
    float2 interleavedPos = fmod(input.myPosition.xy - 0.5f, INTERLEAVED_GRID_SIZE);
#if defined(USE_RANDOM_RAY_SAMPLES)
    float index = (floor(interleavedPos.y) * INTERLEAVED_GRID_SIZE + floor(interleavedPos.x));
    // lightVolumetricRandomRaySamples contains the values 0..63 in a randomized order
    float rayStartOffset = lightVolumetricRandomRaySamples[index] * (stepSize * INTERLEAVED_GRID_SIZE_SQR_RCP);
#else
    float rayStartOffset = (interleavedPos.y * INTERLEAVED_GRID_SIZE + interleavedPos.x) * (stepSize * INTERLEAVED_GRID_SIZE_SQR_RCP);
#endif // USE_RANDOM_RAY_SAMPLES
    
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