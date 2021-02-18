#include "PBRDeferredAmbiance.hlsli"
#include "DeferredPBRFunctions.hlsli"

PixelOutput main(VertexToPixel input)
{
    PixelOutput output;

    float3 exists = PixelShader_Exists(input).myColor.rgb;
    if (length(exists) == 0)
    {
        discard;
    }
        
    float3 worldPosition = PixelShader_WorldPosition(input).myColor.rgb;
    float3 toEye = normalize( cameraPosition.xyz - worldPosition.xyz);
    float3 albedo = GBuffer_Albedo(input).myColor.rgb;
    albedo = GammaToLinear(albedo);
    float3 normal = GBuffer_Normal(input).myColor.xyz;
    float3 vertexNormal = GBuffer_VertexNormal(input).myColor.xyz;
    float ambientOcclusion = GBuffer_AmbientOcclusion(input).myColor.r;
    float metalness = GBuffer_Metalness(input).myColor.r;
    float perceptualRoughness = GBuffer_PerceptualRoughness(input).myColor.r;
    float emissiveData = GBuffer_Emissive(input).myColor.r;
    
    float3 specularColor = lerp((float3) 0.04, albedo, metalness);
    float3 diffuseColor = lerp((float3) 0.00, albedo, 1 - metalness);
    
    float3 ambiance = EvaluateAmbiance(environmentTexture, normal, vertexNormal, toEye, perceptualRoughness, metalness, albedo, ambientOcclusion, diffuseColor, specularColor);
    float3 directionalLight = EvaluateDirectionalLight(diffuseColor, specularColor, normal, perceptualRoughness, directionalLightColor.rgb * directionalLightColor.a, toDirectionalLight.xyz, toEye.xyz);
    float3 emissive = albedo * emissiveData;
    float3 radiance = ambiance + directionalLight + emissive;
    
    output.myColor.rgb = radiance;
    output.myColor.a = 1.0f;
    return output;
}