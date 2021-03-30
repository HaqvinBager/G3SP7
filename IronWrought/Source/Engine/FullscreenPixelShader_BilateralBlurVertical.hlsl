#include "FullscreenShaderStructs.hlsli"

PixelOutput main(VertexToPixel input)
{
    PixelOutput returnValue;
	
    float texelSize = 1.0f / resolution.y;
    float3 blurColor = float3(0.0f, 0.0f, 0.0f);
    float normalizationFactor = 0.0f;
    float bZ = 1.0 / normpdf(0.0, BSIGMA);
    float colorFactor = 0.0f;
    float3 originalPixelValue = fullscreenTexture1.Sample(defaultSampler, input.myUV.xy).rgb;
    
    unsigned int kernelSize = 5;
    float start = (((float) (kernelSize) - 1.0f) / 2.0f) * -1.0f;
    for (unsigned int i = 0; i < kernelSize; i++)
    {
        float2 uv = input.myUV.xy + float2(0.0f, texelSize * (start + (float) i));
        float3 resource = fullscreenTexture1.Sample(defaultSampler, uv).rgb;
        colorFactor = normpdf3(resource - originalPixelValue, BSIGMA) * bZ * gaussianKernel5[i];
        normalizationFactor += colorFactor;
        blurColor += resource * colorFactor/** gaussianKernel5[i]*/;
    }
	
    returnValue.myColor.rgb = blurColor / normalizationFactor;
    returnValue.myColor.a = 1.0f;
    return returnValue;
    
 ////declare stuff
 //   const int kSize = (MSIZE - 1) / 2;
 //   float kernel[MSIZE];
 //   vec3 final_colour = vec3(0.0);
		
	//	//create the 1-D kernel
 //   float Z = 0.0;
 //   for (int j = 0; j <= kSize; ++j)
 //   {
 //       kernel[kSize + j] = kernel[kSize - j] = normpdf(float(j), SIGMA);
 //   }
		
		
 //   vec3 cc;
 //   float factor;
 //   float bZ = 1.0 / normpdf(0.0, BSIGMA);
	//	//read out the texels
 //   for (int i = -kSize; i <= kSize; ++i)
 //   {
 //       for (int j = -kSize; j <= kSize; ++j)
 //       {
 //           cc = texture(iChannel0, vec2(0.0, 1.0) - (fragCoord.xy + vec2(float(i), float(j))) / iResolution.xy).rgb;
 //           factor = normpdf3(cc - c, BSIGMA) * bZ * kernel[kSize + j] * kernel[kSize + i];
 //           Z += factor;
 //           final_colour += factor * cc;

 //       }
 //   }
		
		
 //   fragColor = vec4(final_colour / Z, 1.0);
    
    
};