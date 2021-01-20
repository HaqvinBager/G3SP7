#ifndef DetailNormalHelpers
#define DetailNormalHelpers

#define DETAILNORMAL_TILING 4.0f

float3 SetDetailNormalStrength(float3 detailNormal, float detailNormalStrength, float strengthMultiplier)
{
    float3 returnValue = lerp(float3(0.0, 0.0, 1.0), detailNormal, clamp(detailNormalStrength * strengthMultiplier, 0.0, 4.0));
    return returnValue;
}

//Blending together 2 normals. They should be in 0.0-1.0 range when using this function. https://blog.selfshadow.com/publications/blending-in-detail/
float3 BlendRNM(float3 normal1, float3 normal2)
{
    float3 t = normal1 * float3(2, 2, 2) + float3(-1, -1, 0);
    float3 u = normal2.xyz * float3(-2, -2, 2) + float3(1, 1, -1);
    float3 r = t * dot(t, u) - u * t.z;
    return normalize(r);
}

float DetailStrengthDistanceMultiplier(float3 eyePos, float3 vertWorldPos)
{
    float multiplier = 0.0f;
    float dist = distance(eyePos, vertWorldPos);
    multiplier = clamp((1.0f / (dist * dist)), 0.0f, 4.0f);
    return multiplier;
}

#endif
