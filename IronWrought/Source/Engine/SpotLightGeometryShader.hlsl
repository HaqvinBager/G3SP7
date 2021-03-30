#include "SpotLightShaderStructs.hlsli"

[maxvertexcount(8)]
void main(
	point SpotLightVertexToGeometry input[1],
	inout TriangleStream<SpotLightGeometryToPixel> output
)
{
    const float3 offset[8] =
    {
        { 0.0f, 0.0f, -1.0f },
        { -1.0f, 1.0f, 1.0f },
        { 1.0f, 1.0f, 1.0f },
        { 1.0f, -1.0f, 1.0f },
        { 0.0f, 0.0f, -1.0f },
        { -1.0f, -1.0f, 1.0f },
        { -1.0f, 1.0f, 1.0f },
        { 1.0f, -1.0f, 1.0f }
    };
    
    SpotLightVertexToGeometry inputVertex = input[0];
    inputVertex.myPosition.w = 1.0f;
    
    for (unsigned int index = 0; index < 8; index++)
    {
        SpotLightGeometryToPixel vertex;
        vertex.myPosition = float4(0.0f, 0.0f, 0.0f, 1.0f);
        //vertex.myPosition.z += offset[index].z * spotLightPositionAndRange.w;
        //vertex.myPosition.xy += offset[index].xy * (1.0f / spotLightDirectionAndAngleExponent.w);
        vertex.myPosition.xyz += offset[index].z * spotLightDirectionAndAngleExponent.xyz * spotLightPositionAndRange.w;
        vertex.myPosition.xyz += offset[index].x * spotLightDirectionNormal1.xyz * ( /*2.71828182846f*/3.0f / spotLightDirectionAndAngleExponent.w);
        vertex.myPosition.xyz += offset[index].y * spotLightDirectionNormal2.xyz * ( /*2.71828182846f*/3.0f / spotLightDirectionAndAngleExponent.w);
        //vertex.myPosition.xyz += offset[index].x * spotLightDirectionNormal1.xyz * ( /*2.71828182846f*/pow(2.0f, spotLightDirectionAndAngleExponent.w));
        //vertex.myPosition.xyz += offset[index].y * spotLightDirectionNormal2.xyz * ( /*2.71828182846f*/pow(2.0f, spotLightDirectionAndAngleExponent.w));
        vertex.myPosition = mul(spotLightToWorld, vertex.myPosition);
        vertex.myWorldPosition = vertex.myPosition;
        vertex.myPosition = mul(spotLightToCamera, vertex.myPosition);
        vertex.myPosition = mul(spotLightToProjectionFromCamera, vertex.myPosition);
        vertex.myUV = vertex.myPosition.xyw;
        vertex.myUV.y *= -1.0f;
        output.Append(vertex);
    }
}