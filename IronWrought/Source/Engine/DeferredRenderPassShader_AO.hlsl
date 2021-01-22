#include "PBRDeferredAmbiance.hlsli"
#include "DeferredPBRFunctions.hlsli"

PixelOutput main(VertexToPixel input)
{
    PixelOutput output;

    float ambientOcclusion = GBuffer_AmbientOcclusion(input).myColor.r;
    output.myColor.rgb = ambientOcclusion.xxx;
    output.myColor.a = 1.0f;
    return output;
}