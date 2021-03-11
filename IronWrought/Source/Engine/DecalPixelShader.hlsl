#include "DecalShaderStructs.hlsli"

struct GBufferOutput
{
    float3 myAlbedo         : SV_TARGET0;
    float3 myNormal         : SV_TARGET1;
    float3 myVertexNormal   : SV_TARGET2;
    float4 myMetalRoughAOEm : SV_TARGET3;
};

float4 PixelShader_Normal(float2 uv)
{
    float3 normal;
    normal.xy = normalTexture.Sample(defaultSampler, uv).ag;
    // Recreate z
    normal.z = 0.0f;
    normal = (normal * 2.0f) - 1.0f; 
    normal.z = sqrt(1 - saturate((normal.x * normal.x) + (normal.y * normal.y)));
    normal = normalize(normal);
    
    float4 output;
    output.xyz = normal.xyz;
    output.a = 1.0f;
    return output;
}

GBufferOutput main(VertexToPixel input)
{
    GBufferOutput output;

    float3 clipSpace = input.myClipSpacePosition;
    clipSpace.y *= -1.0f;
    float2 screenSpaceUV = (clipSpace.xy / clipSpace.z) * 0.5f + 0.5f;
    
    float z = depthTexture.Sample(defaultSampler, screenSpaceUV).r;
    float x = screenSpaceUV.x * 2.0f - 1;
    float y = (1 - screenSpaceUV.y) * 2.0f - 1;
    float4 projectedPos = float4(x, y, z, 1.0f);
    float4 viewSpacePos = mul(toCameraFromProjection, projectedPos);
    viewSpacePos /= viewSpacePos.w;
    float4 worldPosFromDepth = mul(toWorldFromCamera, viewSpacePos);
    
    float4 objectPosition = mul(toObjectSpace, worldPosFromDepth);
    clip(0.5f - abs(objectPosition.xyz));
    float2 decalUV = objectPosition.xy + 0.5f;
    decalUV.y *= -1.0f;
    float4 color = colorTexture.Sample(defaultSampler, decalUV).rgba;
    clip(color.a - alphaClipThreshold);
    
    output.myNormal = gBufferNormalTexture.Sample(defaultSampler, screenSpaceUV);
    output.myVertexNormal = gBufferVertexNormalTexture.Sample(defaultSampler, screenSpaceUV);
    output.myMetalRoughAOEm = gBufferMaterialTexture.Sample(defaultSampler, screenSpaceUV);
    
    float3 material = materialTexture.Sample(defaultSampler, decalUV).rgb;
    
    if (length(material))
    {
        float3 normal = PixelShader_Normal(decalUV);
        float3x3 tangentSpaceMatrix = float3x3(normalize(input.myTangent.xyz), normalize(input.myBitangent.xyz), normalize(input.myNormal.xyz));
        normal = mul(normal.xyz, tangentSpaceMatrix);
        normal = normalize(normal);
        output.myNormal = normal;

        float ambientOcclusion = normalTexture.Sample(defaultSampler, decalUV).b;
        float metalness           = material.r;
        float perceptualRoughness = material.g;
        float emissive            = material.b;
        output.myMetalRoughAOEm = float4(metalness, perceptualRoughness, ambientOcclusion, emissive);
    }
      
    output.myAlbedo = color;
    return output;
}