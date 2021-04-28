#include "PBRAmbience.hlsli"
#include "PBRDirectionalLight.hlsli"

PixelOutPut main(VertexToPixel input)
{
    PixelOutPut output;
   
    //float4 normal = PixelShader_TextureNormal(input).myColor.rgba;
    float4 normal = PixelShader_Normal(input).myColor.rgba; 
    float3x3 tangentSpaceMatrix = float3x3(normalize(input.myTangent.xyz), normalize(input.myBiNormal.xyz), normalize(input.myNormal.xyz));
    normal.rgb = mul(normal.xyz, tangentSpaceMatrix);
    normal.rgb = normalize(normal);
    
    output.myColor = normal;
    
    return output;
}