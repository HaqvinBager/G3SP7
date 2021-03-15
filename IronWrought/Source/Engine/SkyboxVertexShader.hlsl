cbuffer TransformBuffer : register(b0)
{
    float4x4 myCameraViewProjection; // Camera view * camera projection (no translation)
}

struct VertToPixel
{
    float3 worldPosition : Position;
    float4 vertexPos : SV_Position;
};

VertToPixel main(float3 pos : Position)
{
    VertToPixel output;
    output.worldPosition = pos;
    output.vertexPos = mul(float4(pos, 0.0f), myCameraViewProjection);
    output.vertexPos.z = output.vertexPos.w; // Make sure that the depth after w divide will be 1.0
    return output;
}