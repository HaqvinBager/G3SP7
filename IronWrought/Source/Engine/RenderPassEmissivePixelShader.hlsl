#include "PBRAmbience.hlsli"
#include "PBRDirectionalLight.hlsli"

PixelOutPut main(VertexToPixel input)
{
    PixelOutPut output;
   
    float4 emissive = PixelShader_Emissive(input).myColor.rgba;
    output.myColor = emissive;
    
    return output;
}