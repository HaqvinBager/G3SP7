#include "SamplingFunctions.hlsli"

PixelOutPut main(VertexToPixel input)
{
    PixelOutPut output;

    float4 albedo = PixelShader_Color(input).myColor;
    clip(albedo.a - 0.5f);

    output.myColor.rgba = 1.0f;

    return output;
}