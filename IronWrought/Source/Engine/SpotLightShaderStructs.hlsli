struct SpotLightVertexInput
{
    float4 myPosition : POSITION;
};

struct SpotLightVertexToGeometry
{
    float4 myPosition : POSITION;
    //float myRange : RANGE;
};

struct SpotLightGeometryToPixel
{
    float4 myPosition : SV_POSITION;
    float3 myUV : UV;
};

struct SpotLightPixelOutput
{
    float4 myColor : SV_TARGET;
};

cbuffer SpotLightFrameBuffer : register(b0)
{
    float4x4 spotLightToCamera;
    float4x4 spotLightToWorldFromCamera;
    float4x4 spotLightToProjection;
    float4x4 spotLightToCameraFromProjection;
    float4 spotLightCameraPosition;
}

cbuffer SpotLightObjectBuffer : register(b1)
{
    float4x4 spotLightToWorld;
    unsigned int spotLightMyNumberOfDetailNormals;
    unsigned int spotLightMyNumberOfTextureSets;
}

cbuffer SpotLightValueBuffer : register(b3)
{
    float4 spotLightColorAndIntensity;
    float4 spotLightPositionAndRange;
    float4 spotLightDirectionAndAngleExponent;
}