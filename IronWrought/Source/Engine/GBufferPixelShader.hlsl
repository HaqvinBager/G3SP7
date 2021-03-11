#include "DeferredSamplingFunctions.hlsli"

struct GBufferOutput
{
    float3 myAlbedo             : SV_TARGET0;
    float3 myNormal             : SV_TARGET1;
    float3 myVertexNormal       : SV_TARGET2;
    float4 myMetalRoughAOEm     : SV_TARGET3;
};

GBufferOutput main(VertexModelToPixel input)
{
    VertexToPixel vertToPixel;
    vertToPixel.myPosition  = input.myPosition;
    vertToPixel.myUV        = input.myUV;
    
    float3 albedo = PixelShader_Albedo(vertToPixel.myUV).rgb;
    float3 normal = normalTexture.Sample(defaultSampler, vertToPixel.myUV).rgb;
    normal = (normal - 0.5f) * 2.0f;
    float3x3 tangentSpaceMatrix = float3x3(normalize(input.myTangent.xyz), normalize(input.myBinormal.xyz), normalize(input.myNormal.xyz));
    normal = mul(normal.xyz, tangentSpaceMatrix);
    normal = normalize(normal);
    
    float ambientOcclusion = normalTexture.Sample(defaultSampler, vertToPixel.myUV).a;
    float metalness = metalnessTexture.Sample(defaultSampler, vertToPixel.myUV);
    float perceptualRoughness = materialTexture.Sample(defaultSampler, vertToPixel.myUV);
    float emissive = /*PixelShader_Emissive(vertToPixel.myUV)*/0.0f;
    
    GBufferOutput output;    
    output.myAlbedo         = albedo.xyz;
    output.myNormal         = normal.xyz;
    output.myVertexNormal   = input.myNormal.xyz;
    output.myMetalRoughAOEm = float4(metalness, perceptualRoughness, ambientOcclusion, emissive);
    
    return output;
}