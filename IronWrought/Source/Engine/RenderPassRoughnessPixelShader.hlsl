#include "PBRAmbience.hlsli"

PixelOutPut main(VertexToPixel input)
{
    PixelOutPut output;
   
    float4 roughness = PixelShader_PerceptualRoughness(input).myColor.rgba;
    output.myColor = roughness;
    
    return output;
}