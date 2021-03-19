#include "DeferredSamplingFunctions.hlsli"
#include "DeferredPBRFunctions.hlsli"
#include "SpotLightShaderStructs.hlsli"

PixelOutput main(SpotLightGeometryToPixel input)
{
    PixelOutput output;
    
    float2 screenUV = (input.myUV.xy / input.myUV.z) * 0.5f + 0.5f;
    
    float depth = PixelShader_Exists(screenUV).r;
    if (depth == 1)
    {
        discard;
    }
    
    float3 worldPosition = PixelShader_WorldPosition(screenUV).rgb;
    float3 toEye = normalize(cameraPosition.xyz - worldPosition.xyz);
    float3 albedo = GBuffer_Albedo(screenUV).rgb;
    albedo = GammaToLinear(albedo);
    float3 normal = GBuffer_Normal(screenUV).xyz;
    float metalness = GBuffer_Metalness(screenUV);
    float perceptualRoughness = GBuffer_PerceptualRoughness(screenUV);
    
    float3 specularColor = lerp((float3) 0.04, albedo, metalness);
    float3 diffuseColor = lerp((float3) 0.00, albedo, 1 - metalness);
    
    float3 toLight = spotLightPositionAndRange.xyz - worldPosition.xyz;
    float lightDistance = length(toLight);
    toLight = normalize(toLight);
    float3 radiance = EvaluateSpotLight(diffuseColor, specularColor, normal, perceptualRoughness, spotLightColorAndIntensity.rgb * spotLightColorAndIntensity.a, spotLightPositionAndRange.w, toLight.xyz, lightDistance, toEye.xyz, spotLightDirectionAndAngleExponent.xyz, spotLightDirectionAndAngleExponent.w);
    
    output.myColor.rgb = radiance;
    output.myColor.a = 1.0f;
    return output;
}