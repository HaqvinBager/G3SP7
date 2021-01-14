#include "ShaderStructs.hlsli"
#include "MathHelpers.hlsli"

PixelOutPut PixelShader_Color(VertexToPixel input)
{
    PixelOutPut output;
    float4 color = colorTexture.Sample(defaultSampler, input.myUV.xy).rgba;
	color.rgb = GammaToLinear(color.rgb);
	output.myColor.rgb = color;
	output.myColor.a = color.a;
    return output;
}

float PixelShader_DetailNormalStrength(VertexToPixel input)
{
    float output = materialTexture.Sample(defaultSampler, input.myUV.xy).a;
    return output;
}

PixelOutPut PixelShader_DetailNormal(VertexToPixel input, int index)
{
    float tilingModifier = 4.0f; // eq to scale
   
    float3 normal;
    normal.xy = detailNormals[index].Sample(defaultSampler, input.myUV.xy * tilingModifier).ag;
    normal.z = 0.0f;
    normal = (normal * 2.0f) - 1.0f;
    normal.z = sqrt(1 - saturate((normal.x * normal.x) + (normal.y * normal.y)));
    normal = normalize(normal);
    
    PixelOutPut output;
    output.myColor.xyz = normal.xyz;
    output.myColor.a = 1.0f;
    return output;
}

PixelOutPut PixelShader_Normal(VertexToPixel input)
{    
    float3 normal; 
    normal.xy = normalTexture.Sample(defaultSampler, input.myUV.xy).ag;
    normal.z = 0.0f;
    normal = (normal * 2.0f) - 1.0f;
    normal.z = sqrt(1 - saturate((normal.x * normal.x) + (normal.y * normal.y)));
    //normal = (normal * 0.5f) + 0.5f;// Found in TGA modelviewer shader code, but seems to cause issues here.
    normal = normalize(normal);
    
    PixelOutPut output;
    
    output.myColor.xyz = normal.xyz;
    output.myColor.a = 1.0f;
    return output;
}

PixelOutPut PixelShader_TextureNormal(VertexToPixel input)
{
    float3 normal = normalTexture.Sample(defaultSampler, input.myUV.xy).agr;
    
    PixelOutPut output;
    output.myColor.xyz = normal.xyz;
    output.myColor.a = 1.0f;
    return output;
}

PixelOutPut PixelShader_AmbientOcclusion(VertexToPixel input)
{
    PixelOutPut output;
    float ao = normalTexture.Sample(defaultSampler, input.myUV.xy).b;
    output.myColor.rgb = ao.xxx;
    output.myColor.a = 1.0f;
    return output;
}

PixelOutPut PixelShader_Material(VertexToPixel input)
{
    PixelOutPut output;
    float3 material = materialTexture.Sample(defaultSampler, input.myUV.xy).rgb;
    output.myColor.rgb = material.rgb;
    output.myColor.a = 1.0f;
    return output;
}

PixelOutPut PixelShader_Metalness(VertexToPixel input)
{
    PixelOutPut output;
    PixelOutPut material = PixelShader_Material(input);
    output.myColor.rgb = material.myColor.rrr;
    output.myColor.a = 1.0f;
    return output;
}

PixelOutPut PixelShader_PerceptualRoughness(VertexToPixel input)
{
    PixelOutPut output;
    PixelOutPut material = PixelShader_Material(input);
    output.myColor.rgb = material.myColor.ggg;
    output.myColor.a = 1.0f;
    return output;
}

PixelOutPut PixelShader_Emissive(VertexToPixel input)
{
    PixelOutPut output;
    PixelOutPut material = PixelShader_Material(input);
    output.myColor.rgb = material.myColor.bbb;
    output.myColor.a = 1.0f;
    return output;
}

