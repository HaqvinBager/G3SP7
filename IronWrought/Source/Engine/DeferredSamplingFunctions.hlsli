#include "DeferredShaderStructs.hlsli"
#include "MathHelpers.hlsli"

PixelOutput PixelShader_WorldPosition(VertexToPixel input)
{
    PixelOutput output;
    float3 worldPos = worldPositionTexture.Sample(defaultSampler, input.myUV.xy).rgb;
    output.myColor.rgb = worldPos;
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
    float3 albedo = albedoTexture.Sample(defaultSampler, input.myUV.xy).rgb;
    output.myColor.rgb = albedo;
    output.myColor.a = 1.0f;
    return output;
}

PixelOutput PixelShader_Normal(VertexToPixel input)
{
    PixelOutput output;
    float3 normal = normalTexture.Sample(defaultSampler, input.myUV.xy).rgb;
    output.myColor.rgb = normal;
    output.myColor.a = 1.0f;
    return output;
}

PixelOutput PixelShader_Material(VertexToPixel input)
{
    PixelOutput output;
    float3 material = materialTexture.Sample(defaultSampler, input.myUV.xy).rgb;
    output.myColor.rgb = material.rgb;
    output.myColor.a = 1.0f;
    return output;
}

PixelOutput PixelShader_AmbientOcclusion(VertexToPixel input)
{
    PixelOutput output;
    float ao = normalTexture.Sample(defaultSampler, input.myUV.xy).a;
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

PixelOutput PixelShader_PerceptualRoughness(VertexToPixel input)
{
    PixelOutput output;
    PixelOutput material = PixelShader_Material(input);
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

// GBUFFER

PixelOutput GBuffer_Albedo(VertexToPixel input)
{
    PixelOutput output;
    float4 albedo = albedoTextureGBuffer.Sample(defaultSampler, input.myUV.xy).rgba;
    output.myColor.rgba = albedo;
    output.myColor.a = 1.0f;
    return output;
}

PixelOutput GBuffer_Normal(VertexToPixel input)
{
    PixelOutput output;
    float3 normal = normalTextureGBuffer.Sample(defaultSampler, input.myUV.xy).rgb;
    output.myColor.rgb = normal;
    output.myColor.a = 1.0f;
    return output;
}

PixelOutput GBuffer_VertexNormal(VertexToPixel input)
{
    PixelOutput output;
    float3 vertexNormal = vertexNormalTexture.Sample(defaultSampler, input.myUV.xy).rgb;
    output.myColor.rgb = vertexNormal;
    output.myColor.a = 1.0f;
    return output;
}

PixelOutput GBuffer_AmbientOcclusion(VertexToPixel input)
{
    PixelOutput output;
    float ao = ambientOcclusionTexture.Sample(defaultSampler, input.myUV.xy).r;
    output.myColor.rgb = ao.xxx;
    output.myColor.a = 1.0f;
    return output;
}

PixelOutput GBuffer_Metalness(VertexToPixel input)
{
    PixelOutput output;
    float metalness = metalnessTexture.Sample(defaultSampler, input.myUV.xy).r;
    output.myColor.rgb = metalness.xxx;
    output.myColor.a = 1.0f;
    return output;
}

PixelOutput GBuffer_PerceptualRoughness(VertexToPixel input)
{
    PixelOutput output;
    float roughness = roughnessTexture.Sample(defaultSampler, input.myUV.xy).r;
    output.myColor.rgb = roughness;
    output.myColor.a = 1.0f;
    return output;
}

PixelOutput GBuffer_Emissive(VertexToPixel input)
{    
    PixelOutput output;
    float emissive = emissiveTexture.Sample(defaultSampler, input.myUV.xy).r;
    output.myColor.rgb = emissive.xxx;
    output.myColor.a = 1.0f;
    return output;
}