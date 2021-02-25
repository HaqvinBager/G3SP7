struct VertexInput {
	unsigned int myIndex : SV_VertexID;
};

struct VertexToPixel {
	float4 myPosition : SV_POSITION;
	float2 myUV : UV;
};

struct PixelOutput {
	float4 myColor : SV_TARGET;
};

struct GBufferOutput {
    float3 myAlbedo         : SV_TARGET0;
    float3 myNormal         : SV_TARGET1;
    float3 myVertexNormal   : SV_TARGET2;
    float4 myMetalRoughAOEm : SV_TARGET3;
};

Texture2D fullscreenTexture1 : register(t0);
Texture2D fullscreenTexture2 : register(t1);
Texture2D fullscreenTexture3 : register(t2);
Texture2D fullscreenTexture4 : register(t3);
Texture2D fullscreenTexture5 : register(t4);
SamplerState defaultSampler : register(s0);

static const float2 resolution = float2(1600.0f, 900.0f) / 8.0f;
static const float gaussianKernel5[5] = { 0.06136f, 0.24477f, 0.38774f, 0.24477f, 0.06136f };