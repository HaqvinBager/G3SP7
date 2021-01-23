#include "PBRDeferredAmbiance.hlsli"
#include "DeferredPBRFunctions.hlsli"

PixelOutput main(VertexToPixel input)
{
    PixelOutput output;
    
    float emissiveData = GBuffer_Emissive(input).myColor.r;
    output.myColor.rgb = emissiveData;
    output.myColor.a = 1.0f;
    return output;
}