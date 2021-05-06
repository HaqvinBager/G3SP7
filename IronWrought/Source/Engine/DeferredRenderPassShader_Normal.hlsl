#include "PBRDeferredAmbiance.hlsli"
#include "DeferredPBRFunctions.hlsli"

PixelOutput main(VertexToPixel input)
{
    PixelOutput output;

    float3 normal = GBuffer_Normal(input.myUV).xyz;
    output.myColor.rgb = normal * 0.5f + 0.5f;
    output.myColor.a = 1.0f;
    return output;
}