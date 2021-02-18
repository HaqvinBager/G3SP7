
#include "DeferredSamplingFunctions.hlsli"
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
    float3 toEye = normalize(cameraPosition.xyz - worldPosition.xyz);
    float3 albedo = GBuffer_Albedo(input).myColor.rgb;
    albedo = GammaToLinear(albedo);
    float3 normal = GBuffer_Normal(input).myColor.xyz;
    float metalness = GBuffer_Metalness(input).myColor.r;
    float perceptualRoughness = GBuffer_PerceptualRoughness(input).myColor.r;
    
    float3 specularColor = lerp((float3) 0.04, albedo, metalness);
    float3 diffuseColor = lerp((float3) 0.00, albedo, 1 - metalness);
    
    float3 toLight = myPositionAndRange.xyz - worldPosition.xyz;
    float lightDistance = length(toLight);
    toLight = normalize(toLight);
    float3 radiance = EvaluatePointLight(diffuseColor, specularColor, normal, perceptualRoughness, myColorAndIntensity.rgb * myColorAndIntensity.a, myPositionAndRange.w, toLight.xyz, lightDistance, toEye.xyz);
    
    output.myColor.rgb = radiance;
    output.myColor.a = 1.0f;
    return output;
}