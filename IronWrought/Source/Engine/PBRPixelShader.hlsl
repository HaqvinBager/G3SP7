#include "DetailNormalHelpers.hlsli"
#include "PBRAmbience.hlsli"
#include "PBRDirectionalLight.hlsli"

float3 EvaluatePointLight(float3 albedoColor, float3 specularColor, float3 normal, float roughness, float3 lightColor, float lightIntensity, float lightRange, float3 lightPos, float3 toEye, float3 pixelPos)
{
    float3 toLight = lightPos.xyz - pixelPos.xyz;
    float lightDistance = length(toLight);
    toLight = normalize(toLight);
    float NdL = saturate(dot(normal, toLight));
    
    float lambert = NdL;
    float NdV = saturate(dot(normal, toEye));
    float3 h = normalize(toLight + toEye);
    float NdH = saturate(dot(normal, h));
    float VdH = saturate(dot(toEye, h));
    float LdV = saturate(dot(toLight, toEye));
    float a = max(0.001f, roughness * roughness);
    
    float3 cDiff = Diffuse(albedoColor);
    float3 cSPec = Specular(specularColor, h, toEye, toLight, a, NdL, NdV, NdH, VdH, LdV);
    
    float linearAttenuation = lightDistance / lightRange;
    linearAttenuation = 1.0f - linearAttenuation;
    linearAttenuation = saturate(linearAttenuation);
    float physicalAttenuation = saturate(1.0f / (lightDistance * lightDistance));
    float attenuation = lambert * linearAttenuation * physicalAttenuation;
    
    return saturate(lightColor * lightIntensity * lambert * linearAttenuation * physicalAttenuation * ((cDiff * (1.0f - cSPec) + cSPec) * PI));
}

PixelOutPut main(VertexToPixel input)
{
    PixelOutPut output;
    
    float3 toEye = normalize(cameraPosition.xyz - input.myWorldPosition.xyz);
    float4 albedo = PixelShader_Color(input).myColor.rgba;
    float3 normal = PixelShader_Normal(input).myColor.xyz;
    
    if (myNumberOfDetailNormals > 0)
    { // get from ModelData when rendering
        float detailNormalStrength = PixelShader_DetailNormalStrength(input);
        float strengthMultiplier = 4.0f; // should change based on distance to camera
        float3 detailNormal;
        for (int i = 0; i < myNumberOfDetailNormals; ++i)
        {
            detailNormal = PixelShader_DetailNormal(input, i).myColor.xyz;
            detailNormal = SetDetailNormalStrength(detailNormal, detailNormalStrength, strengthMultiplier);
            normal = normal * 0.5 + 0.5;
            detailNormal = detailNormal * 0.5 + 0.5;
            normal = BlendRNM(normal, detailNormal);
        }
    } // End of if
    
    float3x3 tangentSpaceMatrix = float3x3(normalize(input.myTangent.xyz), normalize(input.myBiNormal.xyz), normalize(input.myNormal.xyz));
    normal = mul(normal.xyz, tangentSpaceMatrix);
    normal = normalize(normal);
   
    float ambientocclusion = PixelShader_AmbientOcclusion(input).myColor.b;
    float metalness = PixelShader_Metalness(input).myColor.r;
    float perceptualroughness = PixelShader_PerceptualRoughness(input).myColor.g;
    perceptualroughness = pow(abs(perceptualroughness), 1.0f / 1.1f); // from tga modelviewer // To mimic substance painters more blury roughness
    float emissivedata = PixelShader_Emissive(input).myColor.b;
    
    float3 specularcolor = lerp((float3) 0.04, albedo.rgb, metalness);
    float3 color = lerp((float3) 0.00, albedo.rgb, 1 - metalness);
    
    float3 ambience = EvaluateAmbience(environmentTexture, normal, normalize(input.myNormal.xyz), toEye, perceptualroughness, ambientocclusion, color, specularcolor);
    float3 directionallight = EvaluateDirectionalLight(color, specularcolor, normal, perceptualroughness, directionalLightColor.xyz, toDirectionalLight.xyz, toEye.xyz);
    directionallight *= directionalLightColor.w;
 
    float3 pointLights = 0;
    for (unsigned int index = 0; index < myNumberOfUsedPointLights; index++)
    {
        PointLight currentLight = myPointLights[index];
        pointLights += EvaluatePointLight(color, specularcolor, normal, perceptualroughness, currentLight.myColorAndRange.rgb, currentLight.myPositionAndIntensity.w, currentLight.myColorAndRange.a, currentLight.myPositionAndIntensity.xyz, toEye, input.myWorldPosition.xyz);
    }
    
    float3 emissive = albedo * emissivedata; // Maybe add cool multiplier?? // Aki 2021
    float3 radiance = ambience + directionallight + pointLights + emissive;
   
    output.myColor.rgb = LinearToGamma(radiance);
    output.myColor.a = albedo.w;
      
    return output;
}