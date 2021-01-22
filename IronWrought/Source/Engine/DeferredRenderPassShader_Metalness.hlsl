#include "PBRDeferredAmbiance.hlsli"
#include "DeferredPBRFunctions.hlsli"

PixelOutput main(VertexToPixel input)
{
    PixelOutput output;
    
    float metalness = GBuffer_Metalness(input).myColor.r;
    output.myColor.rgb = metalness;
    output.myColor.a = 1.0f;
    return output;
}