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

float3x3 aces_input_matrix =
{
    float3(0.59719f, 0.35458f, 0.04823f),
    float3(0.07600f, 0.90834f, 0.01566f),
    float3(0.02840f, 0.13383f, 0.83777f)
};

float3x3 aces_output_matrix =
{
    float3( 1.60475f, -0.53108f, -0.07367f),
    float3(-0.10208f,  1.10813f, -0.00605f),
    float3(-0.00327f, -0.07276f,  1.07602f)
};

//vec3 mul(const std::array< vec3, 3>& m, const vec3& v)
//{
//    float x = m[0][0] * v[0] + m[0][1] * v[1] + m[0][2] * v[2];
//    float y = m[1][0] * v[1] + m[1][1] * v[1] + m[1][2] * v[2];
//    float z = m[2][0] * v[1] + m[2][1] * v[1] + m[2][2] * v[2];
//    return vec3(x, y, z);
//}

float3 rtt_and_odt_fit(float3 v)
{
    float3 a = v * (v + 0.0245786f) - 0.000090537f;
    float3 b = v * (0.983729f * v + 0.4329510f) + 0.238081f;
    return a / b;
}

float3 aces_fitted(float3 v)
{
    v = mul(aces_input_matrix, v);
    v = rtt_and_odt_fit(v);
    return mul(aces_output_matrix, v);
}

PixelOutput main(VertexToPixel input)
{
    PixelOutput returnValue;
    float3 resource = fullscreenTexture1.Sample(defaultSampler, input.myUV.xy).rgb;
	
	// No Tonemapping
	//{
 //       returnValue.myColor.rgb = resource;
 //   }
	// No Tonemapping

	// Reinhard
	//{
 //       returnValue.myColor.rgb = resource / (resource + 1.0f);
 //   }
	// Reinhard
    
	// Uncharted 2
	{
        float3 whitePoint = 10.0f;
        float exposure = 1.0f;
        returnValue.myColor.rgb = Uncharted2Tonemap(resource * exposure) / Uncharted2Tonemap(whitePoint);
    }
	// Uncharted 2
    
    // ACES
    //{
    //    float3 whitePoint = 10.0f;
    //    float exposure = 3.0f;
    //    returnValue.myColor.rgb = aces_fitted(resource * exposure) / aces_fitted(whitePoint);
    //}
    // ACES
	
	
    returnValue.myColor.a = 0.0f;
    return returnValue;
};