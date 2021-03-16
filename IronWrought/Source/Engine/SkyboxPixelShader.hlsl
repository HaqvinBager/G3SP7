TextureCube cubemapTexture : register(t0);
SamplerState defaultSampler : register(s0);

float4 main(float3 worldPosition : Position) : SV_TARGET
{
    return cubemapTexture.Sample(defaultSampler, worldPosition);
}