struct VertexInput
{
    float4 myPosition   : POSITION;
    float4 myNormal     : NORMAL;
    float4 myTangent    : TANGENT;
    float4 myBitangent  : BITANGENT;
    float2 myUV         : UV;
};

struct VertexToPixel
{
    float4 myPosition           : SV_Position;
    float3 myClipSpacePosition  : CLIP_SPACE_POSITION;
    float4 myNormal             : NORMAL;
    float4 myTangent            : TANGENT;
    float4 myBitangent          : BITANGENT;
    float2 myUV                 : UV;
};

Texture2D colorTexture      : register(t0);
Texture2D materialTexture   : register(t1);
Texture2D normalTexture     : register(t2);
Texture2D depthTexture      : register(t21);
SamplerState defaultSampler : register(s0);

cbuffer FrameBuffer : register(b0)
{
    float4x4 toCameraSpace;
    float4x4 toProjectionSpace;
    float4x4 toWorldFromCamera;
    float4x4 toCameraFromProjection;
};

cbuffer ObjectBuffer : register(b1)
{
    float4x4 toWorld;
    float4x4 toObjectSpace;
    float alphaClipThreshold;
    float3 padding;
};