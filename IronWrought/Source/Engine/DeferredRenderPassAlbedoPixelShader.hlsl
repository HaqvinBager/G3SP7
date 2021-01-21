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
    
    float3 albedo = PixelShader_Albedo(vertToPixel).myColor.rgb;
    float3 normal = PixelShader_Normal(vertToPixel).myColor.xyz;
    
    if (myNumberOfDetailNormals > 0)
    {
        float detailNormalStrength = PixelShader_DetailNormalStrength(vertToPixel);
        float strengthMultiplier = DetailStrengthDistanceMultiplier(cameraPosition.xyz, input.myWorldPosition.xyz); // should change based on distance to camera
        float3 detailNormal;

        // Blend based on detail normal strength
        // X3512 Sampler array index must be literal expression => DETAILNORMAL_#
        // Make this better please
        int dnIndex = DetailNormalToBlend(myNumberOfDetailNormals, detailNormalStrength);// This is an if-chain. D:
        if (dnIndex == DETAILNORMAL_1)// This is also an if-chain >:/
            detailNormal = PixelShader_DetailNormal(vertToPixel, DETAILNORMAL_1).myColor.xyz;
        else if (dnIndex == DETAILNORMAL_2)
            detailNormal = PixelShader_DetailNormal(vertToPixel, DETAILNORMAL_2).myColor.xyz;
        else if (dnIndex == DETAILNORMAL_3)
            detailNormal = PixelShader_DetailNormal(vertToPixel, DETAILNORMAL_3).myColor.xyz;
        else if (dnIndex == DETAILNORMAL_4)
            detailNormal = PixelShader_DetailNormal(vertToPixel, DETAILNORMAL_4).myColor.xyz;
        
        detailNormal = SetDetailNormalStrength(detailNormal, detailNormalStrength, strengthMultiplier);
        normal = normal * 0.5 + 0.5;
        detailNormal = detailNormal * 0.5 + 0.5;
        normal = BlendRNM(normal, detailNormal);
        
        //---
        
        // Blend all 4 ontop of eachother
        //for (int i = 0; i < myNumberOfDetailNormals; ++i)
        //{
        //    detailNormal = PixelShader_DetailNormal(vertToPixel, i).myColor.xyz;
        //    detailNormal = SetDetailNormalStrength(detailNormal, detailNormalStrength, strengthMultiplier);
        //    normal = normal * 0.5 + 0.5;
        //    detailNormal = detailNormal * 0.5 + 0.5;
        //    normal = BlendRNM(normal, detailNormal);
        //}
    } // End of if
    
    float3x3 tangentSpaceMatrix = float3x3(normalize(input.myTangent.xyz), normalize(input.myBinormal.xyz), normalize(input.myNormal.xyz));
    normal = mul(normal.xyz, tangentSpaceMatrix);
    normal = normalize(normal);
    
    float ambientOcclusion      = PixelShader_AmbientOcclusion(vertToPixel).myColor.r;
    float metalness             = PixelShader_Metalness(vertToPixel).myColor.r;
    float perceptualRoughness   = PixelShader_PerceptualRoughness(vertToPixel).myColor.r;
    float emissive              = PixelShader_Emissive(vertToPixel).myColor.r;
    
    // Using 4 textures
    GBufferOutput output;
    output.myWorldPosition  = input.myWorldPosition;
    output.myAlbedo         = float4(albedo, 1.0f);
    output.myNormal         = float4(normal, 1.0f);
    output.myVertexNormal   = float4(input.myNormal.xyz, 1.0f);
    
    output.myWorldPosition.w = metalness;
    output.myAlbedo.w        = perceptualRoughness;
    output.myNormal.w        = ambientOcclusion;
    output.myVertexNormal    = emissive;
    return output;
}