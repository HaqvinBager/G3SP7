#include "PBRDeferredAmbiance.hlsli"
#include "DeferredPBRFunctions.hlsli"

PixelOutput main(VertexToPixel input)
{
    PixelOutput output;

    float3 albedo = GBuffer_Albedo(input).myColor.rgb;    
    output.myColor.rgb = albedo;
    output.myColor.a = 1.0f;
    return output;
}