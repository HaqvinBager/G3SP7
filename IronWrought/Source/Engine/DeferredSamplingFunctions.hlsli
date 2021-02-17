#include "DeferredShaderStructs.hlsli"
#include "MathHelpers.hlsli"
#include "DetailNormalHelpers.hlsli"

PixelOutput PixelShader_WorldPosition(VertexToPixel input)
{
    PixelOutput output;
    
    // World Pos texture // Deprecated
    //float4 worldPos = worldPositionTexture.Sample(defaultSampler, input.myUV.xy).rgba;
    
    // Depth sampling
    float z = depthTexture.Sample(defaultSampler, input.myUV.xy).r;
    float x = input.myUV.x * 2.0f - 1;
    float y = (1 - input.myUV.y) * 2.0f - 1;
    float4 projectedPos = float4(x, y, z, 1.0f);
    float4 viewSpacePos = mul(toCameraFromProjection, projectedPos);
    viewSpacePos /= viewSpacePos.w;
    float4 worldPos = mul(toWorldFromCamera, viewSpacePos);

    output.myColor.rgb = worldPos.rgb;
    output.myColor.a = 1.0f;
    return output;
}

PixelOutput PixelShader_Exists(VertexToPixel input)
{
    PixelOutput output;
    float4 worldPos = PixelShader_WorldPosition(input).myColor.rgba;
    output.myColor.rgba = worldPos;
    return output;
}

PixelOutput PixelShader_Albedo(VertexToPixel input)
{
    PixelOutput output;
    float4 color = albedoTexture.Sample(defaultSampler, input.myUV.xy).rgba;
    //color.rgb = GammaToLinear(color.rgb);// Used for when testing Detail normals, tufted leather model had incorrect compression format (sLinear instead of sRGB) or something lika that i don't remember
    output.myColor.rgb = color.rgb;
    output.myColor.a = color.a;
    return output;
}

PixelOutput PixelShader_Albedo(Texture2D anAlbedoTexture, VertexToPixel input)
{
    PixelOutput output;
    float4 color = anAlbedoTexture.Sample(defaultSampler, input.myUV.xy).rgba;
    output.myColor.rgb = color.rgb;
    output.myColor.a = color.a;
    return output;
}

// This function is used for renderpasses to isolate the normal texture.
PixelOutput PixelShader_NormalForIsolatedRendering(VertexToPixel input)
{
    float3 normal;
    normal.xy = normalTexture.Sample(defaultSampler, input.myUV.xy).ag;
    // Recreate z
    normal.z = 0.0f;
    normal.z = sqrt(1 - saturate((normal.x * normal.x) + (normal.y * normal.y)));
    normal = normalize(normal);
    
    PixelOutput output;
    output.myColor.xyz = normal.xyz;
    output.myColor.a = 1.0f;
    return output;
}

PixelOutput PixelShader_Normal(VertexToPixel input)
{
    float3 normal;
    normal.xy = normalTexture.Sample(defaultSampler, input.myUV.xy).ag;
    // Recreate z
    normal.z = 0.0f;
    normal = (normal * 2.0f) - 1.0f;// Comment this for Normal shader render pass
    normal.z = sqrt(1 - saturate((normal.x * normal.x) + (normal.y * normal.y)));
    //normal = (normal * 0.5f) + 0.5f;// Found in TGA modelviewer shader code, but seems to cause issues here.
    normal = normalize(normal);
    
    PixelOutput output;
    output.myColor.xyz = normal.xyz;
    output.myColor.a = 1.0f;
    return output;
}

PixelOutput PixelShader_Normal(Texture2D aNormalTexture, VertexToPixel input)
{
    float3 normal;
    normal.xy = aNormalTexture.Sample(defaultSampler, input.myUV.xy).ag;
    // Recreate z
    normal.z = 0.0f;
    normal = (normal * 2.0f) - 1.0f; // Comment this for Normal shader render pass
    normal.z = sqrt(1 - saturate((normal.x * normal.x) + (normal.y * normal.y)));
    //normal = (normal * 0.5f) + 0.5f;// Found in TGA modelviewer shader code, but seems to cause issues here.
    normal = normalize(normal);
    
    PixelOutput output;
    output.myColor.xyz = normal.xyz;
    output.myColor.a = 1.0f;
    return output;
}

float PixelShader_DetailNormalStrength(VertexToPixel input)
{
    float output = materialTexture.Sample(defaultSampler, input.myUV.xy).a;
    return output;
}
PixelOutput PixelShader_DetailNormal(VertexToPixel input, int index)
{
    float tilingModifier = DETAILNORMAL_TILING; // eq to scale
   
    float3 normal;
    
    normal.xy = detailNormals[index].Sample(defaultSampler, input.myUV.xy * tilingModifier).ag;
    normal.z = 0.0f;
    normal = (normal * 2.0f) - 1.0f;
    normal.z = sqrt(1 - saturate((normal.x * normal.x) + (normal.y * normal.y)));
    normal = normalize(normal);
    
    PixelOutput output;
    output.myColor.xyz = normal.xyz;
    output.myColor.a = 1.0f;
    return output;
}

PixelOutput PixelShader_Material(VertexToPixel input)
{
    PixelOutput output;
    float4 material = materialTexture.Sample(defaultSampler, input.myUV.xy).rgba;
    output.myColor.rgb = material.rgb;
    output.myColor.a = material.a;
    return output;
}

PixelOutput PixelShader_Material(Texture2D aMaterialTexture, VertexToPixel input)
{
    PixelOutput output;
    float4 material = aMaterialTexture.Sample(defaultSampler, input.myUV.xy).rgba;
    output.myColor.rgb = material.rgb;
    output.myColor.a = material.a;
    return output;
}

PixelOutput PixelShader_AmbientOcclusion(VertexToPixel input)
{
    PixelOutput output;
    float ao = normalTexture.Sample(defaultSampler, input.myUV.xy).b;
    output.myColor.rgb = ao.xxx;
    output.myColor.a = 1.0f;
    return output;
}

PixelOutput PixelShader_AmbientOcclusion(Texture2D aNormalTexture, VertexToPixel input)
{
    PixelOutput output;
    float ao = aNormalTexture.Sample(defaultSampler, input.myUV.xy).b;
    output.myColor.rgb = ao.xxx;
    output.myColor.a = 1.0f;
    return output;
}

PixelOutput PixelShader_Metalness(VertexToPixel input)
{
    PixelOutput output;
    PixelOutput material = PixelShader_Material(input);
    output.myColor.rgb = material.myColor.rrr;
    output.myColor.a = 1.0f;
    return output;
}

PixelOutput PixelShader_Metalness(Texture2D aMaterialTexture, VertexToPixel input)
{
    PixelOutput output;
    PixelOutput material = PixelShader_Material(aMaterialTexture, input);
    output.myColor.rgb = material.myColor.rrr;
    output.myColor.a = 1.0f;
    return output;
}

PixelOutput PixelShader_PerceptualRoughness(VertexToPixel input)
{
    PixelOutput output;
    PixelOutput material = PixelShader_Material(input);
    output.myColor.rgb = material.myColor.ggg;
    output.myColor.a = 1.0f;
    return output;
}

PixelOutput PixelShader_PerceptualRoughness(Texture2D aMaterialTexture, VertexToPixel input)
{
    PixelOutput output;
    PixelOutput material = PixelShader_Material(aMaterialTexture, input);
    output.myColor.rgb = material.myColor.ggg;
    output.myColor.a = 1.0f;
    return output;
}

PixelOutput PixelShader_Emissive(VertexToPixel input)
{
    PixelOutput output;
    PixelOutput material = PixelShader_Material(input);
    output.myColor.rgb = material.myColor.bbb;
    output.myColor.a = 1.0f;
    return output;
}

PixelOutput PixelShader_Emissive(Texture2D aMaterialTexture, VertexToPixel input)
{
    PixelOutput output;
    PixelOutput material = PixelShader_Material(aMaterialTexture, input);
    output.myColor.rgb = material.myColor.bbb;
    output.myColor.a = 1.0f;
    return output;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
// GBUFFER  GBUFFER GBUFFER GBUFFER GBUFFER GBUFFER GBUFFER GBUFFER GBUFFER GBUFFER GBUFFER
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

PixelOutput GBuffer_Albedo(VertexToPixel input)
{
    PixelOutput output;
    float3 albedo = albedoTextureGBuffer.Sample(defaultSampler, input.myUV.xy).rgb;
    output.myColor.rgb = albedo;
    output.myColor.a = 1.0f;
    return output;
}

PixelOutput GBuffer_Normal(VertexToPixel input)
{
    PixelOutput output;
    float3 normal = normalTextureGBuffer.Sample(defaultSampler, input.myUV.xy).rgb;

// Recreate z
    // Fråga björn 18/2 :)
    //float3 normal;
    //normal.xy = normalTextureGBuffer.Sample(defaultSampler, input.myUV.xy).wy;
    //normal.z = 0.0f;
    //normal.z = sqrt(1.f - saturate((normal.x * normal.x) + (normal.y * normal.y)));   
    //normal.z = sqrt(1.f - (normal.x * normal.x) - (normal.y * normal.y));// Unity
    // random tests to see what works
        //normal = normalize(normal);
        //normal.z *= -1.0f;
        //normal = normal * 0.5 + 0.5;// dont do this
    
    output.myColor.rgb = normal;
    output.myColor.a = 1.0f;
    return output;
}

PixelOutput GBuffer_VertexNormal(VertexToPixel input)
{
    PixelOutput output;
    float3 vertexNormal = vertexNormalTextureGBuffer.Sample(defaultSampler, input.myUV.xy).rgb;
    output.myColor.rgb = vertexNormal;
    output.myColor.a = 1.0f;
    return output;
}

PixelOutput GBuffer_AmbientOcclusion(VertexToPixel input)
{
    PixelOutput output;
    float ao = materialTextureGBuffer.Sample(defaultSampler, input.myUV.xy).b;
    output.myColor.rgb = ao.xxx;
    output.myColor.a = 1.0f;
    return output;
}

PixelOutput GBuffer_Metalness(VertexToPixel input)
{
    PixelOutput output;
    float metalness = materialTextureGBuffer.Sample(defaultSampler, input.myUV.xy).r;
    output.myColor.rgb = metalness.xxx;
    output.myColor.a = 1.0f;
    return output;
}

PixelOutput GBuffer_PerceptualRoughness(VertexToPixel input)
{
    PixelOutput output;
    float roughness = materialTextureGBuffer.Sample(defaultSampler, input.myUV.xy).g;
    output.myColor.rgb = roughness;
    output.myColor.a = 1.0f;
    return output;
}

PixelOutput GBuffer_Emissive(VertexToPixel input)
{   
    PixelOutput output;
    float emissive = materialTextureGBuffer.Sample(defaultSampler, input.myUV.xy).a;
    output.myColor.rgb = emissive.xxx;
    output.myColor.a = 1.0f;
    return output;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
// SHADOWS SHADOWS SHADOWS SHADOWS SHADOWS SHADOWS SHADOWS SHADOWS SHADOWS SHADOWS SHADOWS
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

float SampleShadowPos(float3 projectionPos)
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

    if (b - a < 0.001f)
    {
        return 0.0f;
    }
    else
    {
        return 1.0f;
    }
}

float3 ShadowFactor(float3 worldPosition)
{
    worldPosition -= directionalLightPosition.xyz;
    float4 viewPos = mul(worldPosition, toDirectionalLightTransform);
    float4 projectionPos = mul(viewPos, toDirectionalLightView);
    float3 viewCoords = projectionPos.xyz;

    float total = 0.0f;
    for (float x = -1.0; x < 1.5f; x += 1.0f)
    {
        for (float y = -1.0; y < 1.5f; y += 1.0f)
        {
            //2048.0f * 4.0f,
            float3 off;
            off.x = x / (2048.0f * 4.0f);
            off.y = y / (2048.0f * 4.0f);
            off.z = 0.0f;
            total += SampleShadowPos(viewCoords + off);
        }
    }
    total /= 9.0f;
    return total;
}