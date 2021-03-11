#include "DeferredSamplingFunctions.hlsli"

struct GBufferOutput
{
    float4 myWorldPosition      : SV_TARGET0;
    float4 myAlbedo             : SV_TARGET1;
    float4 myNormal             : SV_TARGET2;
    float4 myVertexNormal       : SV_TARGET3;
};

GBufferOutput main(VertexModelToPixel input)
{
    VertexToPixel vertToPixel;
    vertToPixel.myPosition  = input.myPosition;
    vertToPixel.myUV        = input.myUV;
    
    float3 albedo = PixelShader_Albedo(vertToPixel.myUV).rgb;
    float3 normal = PixelShader_NormalForIsolatedRendering(vertToPixel.myUV).xyz;
    
    float ambientOcclusion      = PixelShader_AmbientOcclusion(vertToPixel.myUV);
    float metalness             = PixelShader_Metalness(vertToPixel.myUV);
    float perceptualRoughness   = PixelShader_PerceptualRoughness(vertToPixel.myUV);
    float emissive              = PixelShader_Emissive(vertToPixel.myUV);
    
    // Using 4 textures
    GBufferOutput output;
    output.myWorldPosition  = input.myWorldPosition;
    output.myAlbedo         = float4(albedo, 1.0f);
    output.myNormal         = float4(normal, 1.0f);
    output.myVertexNormal   = float4(input.myNormal.xyz, 1.0f);
    
    output.myWorldPosition.w = metalness;
    output.myAlbedo.w        = perceptualRoughness;
    output.myNormal.w        = ambientOcclusion;
    output.myVertexNormal.w    = emissive;
    return output;
}