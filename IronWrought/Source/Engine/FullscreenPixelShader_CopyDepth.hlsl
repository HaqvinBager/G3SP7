#include "FullscreenShaderStructs.hlsli"

float3 LinearToGamma(float3 aColor)
{
    return pow(abs(aColor), 1.0 / 2.2);
}

float3 GammaToLinear(float3 aColor)
{
    return pow(abs(aColor), 2.2);
}

PixelOutput main(VertexToPixel input)
{
    PixelOutput returnValue;
    float resource = fullscreenTexture1.Sample(defaultSampler, input.myUV.xy).r;
    returnValue.myColor.r = resource;
    //returnValue.myColor.r = LinearToGamma(resource);
    returnValue.myColor.a = 1.0f;
    return returnValue;
}