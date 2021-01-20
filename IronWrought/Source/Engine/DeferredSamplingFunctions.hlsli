#include "DeferredShaderStructs.hlsli"
#include "MathHelpers.hlsli"
#include "DetailNormalHelpers.hlsli"

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
    float4 color = albedoTexture.Sample(defaultSampler, input.myUV.xy).rgba;
    //color.rgb = GammaToLinear(color.rgb);// Used for when testing Detail normals, tufted leather model had incorrect compression format (sLinear instead of sRGB)
    output.myColor.rgb = color;
    output.myColor.a = color.a;
    return output;
}

PixelOutput PixelShader_Normal(VertexToPixel input)
{
    float3 normal;
    normal.xy = normalTexture.Sample(defaultSampler, input.myUV.xy).ag;
    // Recreate z
    normal.z = 0.0f;
    normal = (normal * 2.0f) - 1.0f;
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
    float3 material = materialTexture.Sample(defaultSampler, input.myUV.xy).rgb;
    output.myColor.rgb = material.rgb;
    output.myColor.a = 1.0f;
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