#include "PBRDeferredAmbiance.hlsli"
#include "DeferredPBRFunctions.hlsli"

PixelOutput main(VertexToPixel input)
{
    PixelOutput output;

    float perceptualRoughness = GBuffer_PerceptualRoughness(input.myUV);
    output.myColor.rgb = perceptualRoughness;
    output.myColor.a = 1.0f;
    return output;
}