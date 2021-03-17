struct VertexInput
{
	float4 myPosition		:	POSITION;
	float4 myColor			:	COLOR;
	float2 myUV				:	UV;
};

struct VertexToPixel
{
	float4 myPosition		:	SV_POSITION;
	float4 myColor			:	COLOR;
	float2 myUV				:	UV;
};

struct PixelOutput
{
	float4 myColor			:	SV_TARGET;
};

cbuffer FrameBuffer : register(b0)
{
	float4x4 toCamera;
	float4x4 toProjection;
};

cbuffer ObjectBuffer : register(b1)
{
	float4x4 toWorld;
};

TextureCube environmentTexture : register(t0);
Texture2D instanceTexture	: register(t1);
SamplerState defaultSampler	: register(s0);