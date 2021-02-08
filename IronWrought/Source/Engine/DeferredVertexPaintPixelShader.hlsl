#include "DeferredSamplingFunctions.hlsli"

struct GBufferOutput
{
    float4 myWorldPosition  : SV_TARGET0;
    float4 myAlbedo         : SV_TARGET1;
    float4 myNormal         : SV_TARGET2;
    float4 myVertexNormal   : SV_TARGET3;
};

GBufferOutput main(VertexPaintModelToPixel input)
{
    VertexToPixel vertToPixel;
    vertToPixel.myPosition = input.myPosition;
    vertToPixel.myUV = input.myUV;
    
    float3 vertexColors = input.myColor;
    
    float3 albedo               = PixelShader_Albedo(vertToPixel).myColor.rgb;
    float3 normal               = PixelShader_Normal(vertToPixel).myColor.xyz;
    float3x3 tangentSpaceMatrix = float3x3(normalize(input.myTangent.xyz), normalize(input.myBinormal.xyz), normalize(input.myNormal.xyz));
    normal = mul(normal.xyz, tangentSpaceMatrix);
    normal = normalize(normal);
    float ambientOcclusion      = PixelShader_AmbientOcclusion(vertToPixel).myColor.r;
    float metalness             = PixelShader_Metalness(vertToPixel).myColor.r;
    float perceptualRoughness   = PixelShader_PerceptualRoughness(vertToPixel).myColor.r;
    float emissive              = PixelShader_Emissive(vertToPixel).myColor.r;
   
    // Extra materials
    float3 albedo2              = PixelShader_Albedo(vertexPaintTextures[RED_ALBEDO], vertToPixel).myColor.rgb;
    float3 normal2              = PixelShader_Normal(vertexPaintTextures[RED_NORMAL], vertToPixel).myColor.xyz;
    normal2 = mul(normal2.xyz, tangentSpaceMatrix);
    normal2 = normalize(normal2);
    float ambientOcclusion2     = PixelShader_AmbientOcclusion(vertexPaintTextures[RED_NORMAL], vertToPixel).myColor.r;
    float metalness2            = PixelShader_Metalness(vertexPaintTextures[RED_MATERIAL], vertToPixel).myColor.r;
    float perceptualRoughness2  = PixelShader_PerceptualRoughness(vertexPaintTextures[RED_MATERIAL], vertToPixel).myColor.r;
    float emissive2             = PixelShader_Emissive(vertexPaintTextures[RED_MATERIAL], vertToPixel).myColor.r;
    
    float3 albedo3              = PixelShader_Albedo(vertexPaintTextures[GREEN_ALBEDO], vertToPixel).myColor.rgb;
    float3 normal3              = PixelShader_Normal(vertexPaintTextures[GREEN_NORMAL], vertToPixel).myColor.xyz;
    normal3 = mul(normal3.xyz, tangentSpaceMatrix);
    normal3 = normalize(normal3);
    float ambientOcclusion3     = PixelShader_AmbientOcclusion(vertexPaintTextures[GREEN_NORMAL], vertToPixel).myColor.r;
    float metalness3            = PixelShader_Metalness(vertexPaintTextures[GREEN_MATERIAL], vertToPixel).myColor.r;
    float perceptualRoughness3  = PixelShader_PerceptualRoughness(vertexPaintTextures[GREEN_MATERIAL], vertToPixel).myColor.r;
    float emissive3             = PixelShader_Emissive(vertexPaintTextures[GREEN_MATERIAL], vertToPixel).myColor.r;
    
    float3 albedo4              = PixelShader_Albedo(vertexPaintTextures[BLUE_ALBEDO], vertToPixel).myColor.rgb;
    float3 normal4              = PixelShader_Normal(vertexPaintTextures[BLUE_NORMAL], vertToPixel).myColor.xyz;
    normal4 = mul(normal4.xyz, tangentSpaceMatrix);
    normal4 = normalize(normal4);
    float ambientOcclusion4     = PixelShader_AmbientOcclusion(vertexPaintTextures[BLUE_NORMAL], vertToPixel).myColor.r;
    float metalness4            = PixelShader_Metalness(vertexPaintTextures[BLUE_MATERIAL], vertToPixel).myColor.r;
    float perceptualRoughness4  = PixelShader_PerceptualRoughness(vertexPaintTextures[BLUE_MATERIAL], vertToPixel).myColor.r;
    float emissive4             = PixelShader_Emissive(vertexPaintTextures[BLUE_MATERIAL], vertToPixel).myColor.r;
    
    // Using 4 textures
    GBufferOutput output;
    output.myWorldPosition = input.myWorldPosition;
    output.myAlbedo = float4(lerp(lerp(lerp(albedo, albedo2, vertexColors.r), albedo3, vertexColors.g), albedo4, vertexColors.b), 1.0f);
    output.myNormal = float4(lerp(lerp(lerp(normal, normal2, vertexColors.r), normal3, vertexColors.g), normal4, vertexColors.b), 1.0f);
    output.myVertexNormal = float4(input.myNormal.xyz, 1.0f);
    
    output.myWorldPosition.w = lerp(lerp(lerp(metalness, metalness2, vertexColors.r), metalness3, vertexColors.g), metalness4, vertexColors.b);
    output.myAlbedo.w = lerp(lerp(lerp(perceptualRoughness, perceptualRoughness2, vertexColors.r), perceptualRoughness3, vertexColors.g), perceptualRoughness4, vertexColors.b);
    output.myNormal.w = lerp(lerp(lerp(ambientOcclusion, ambientOcclusion2, vertexColors.r), ambientOcclusion3, vertexColors.g), ambientOcclusion4, vertexColors.b);
    output.myVertexNormal = lerp(lerp(lerp(emissive, emissive2, vertexColors.r), emissive3, vertexColors.g), emissive4, vertexColors.b);
    return output;
}