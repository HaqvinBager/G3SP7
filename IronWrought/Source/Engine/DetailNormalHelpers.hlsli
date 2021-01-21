#ifndef DetailNormalHelpers
#define DetailNormalHelpers

#define DETAILNORMAL_TILING 4.0f

#define DETAILNORMAL_1 0
#define DETAILNORMAL_2 1
#define DETAILNORMAL_3 2
#define DETAILNORMAL_4 3

#define DETAILNORMAL_DISTANCE_MAX 1.0f /* Don't go higher than 4.0f. Gives no noticable effect. */

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
    multiplier = clamp((1.0f / (dist * dist)), 0.0f, DETAILNORMAL_DISTANCE_MAX);
    return multiplier;
}

int DetailNormalToBlend(int aNumberOfDetailNormals, float detailNormalStrength)
{
    int index = DETAILNORMAL_1;
    // Lazy way
    //if (detailNormalStrength > 0.0f && detailNormalStrength < 0.23f)
    //    index = DETAILNORMAL_1;
    //else if (detailNormalStrength > 0.26f && detailNormalStrength < 0.48f)
    //    index = DETAILNORMAL_2;
    //else if (detailNormalStrength > 0.51f && detailNormalStrength < 0.73f)
    //    index = DETAILNORMAL_3;
    //else if (detailNormalStrength > 0.76f && detailNormalStrength < 1.0f)
    //    index = DETAILNORMAL_4;
    
    // Lazy way : alt 2
    if (detailNormalStrength > 0.76f)
        index = DETAILNORMAL_4;
    else if (detailNormalStrength > 0.51f)
        index = DETAILNORMAL_3;
    else if (detailNormalStrength > 0.26f)
        index = DETAILNORMAL_2;
    //else if (detailNormalStrength > 0.0f)
    //    index = DETAILNORMAL_1;
    
    index = index > aNumberOfDetailNormals ? aNumberOfDetailNormals : index;
    return index;
}

#endif
