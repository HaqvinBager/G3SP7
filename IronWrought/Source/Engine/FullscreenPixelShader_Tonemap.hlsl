#include "FullscreenShaderStructs.hlsli"

float3 Uncharted2Tonemap(float3 x)
{
    float A = 0.15f;
    float B = 0.5f;
    float C = 0.1f;
    float D = 0.2f;
    float E = 0.02f;
    float F = 0.3f;
    
    return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

PixelOutput main(VertexToPixel input)
{
    PixelOutput returnValue;
    float3 resource = fullscreenTexture1.Sample(defaultSampler, input.myUV.xy).rgb;
	
	// Reinhard
	//{
 //       returnValue.myColor.rgb = resource / (resource + 1.0f);
 //   }
	// Reinhard
    
	// Uncharted 2
	{
        float3 whitePoint = 10.0f;
        float exposure = 3.0f;
        returnValue.myColor.rgb = Uncharted2Tonemap(resource * exposure) / Uncharted2Tonemap(whitePoint);
    }
	// Uncharted 2
	
	// No Tonemapping
	//{
 //       returnValue.myColor.rgb = resource;
 //   }
	// No Tonemapping
	
    returnValue.myColor.a = 0.0f;
    return returnValue;
};