struct BoxLightVertexInput
{
    float4 myPosition : POSITION;
};

struct BoxLightVertexToPixel
{
    float4 myPosition : SV_POSITION;
    float4 myWorldPosition : WORLD_POSITION;
    float3 myUV : UV;
};

struct BoxLightPixelOutput
{
    float4 myColor : SV_TARGET;
};

cbuffer BoxLightFrameBuffer : register(b0)
{
    float4x4 boxLightToCamera;
    float4x4 boxLightToWorldFromCamera;
    float4x4 boxLightToProjection;
    float4x4 boxLightToCameraFromProjection;
    float4 boxLightCameraPosition;
}

cbuffer BoxLightObjectBuffer : register(b3)
{
    float4x4 toBoxLightWorld;
    float4x4 toBoxLightView;
    float4x4 toBoxLightProjection;
    float4 boxLightColorAndIntensity;
    float4 boxLightPositionAndRange;
    float4 boxLightDirection;
    //float2 boxLightWidthAndHeight;
    //float2 boxLightPadding;
}