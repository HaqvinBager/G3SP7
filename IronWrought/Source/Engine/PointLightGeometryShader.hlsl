#include "PointLightShaderStructs.hlsli"

[maxvertexcount(4)]
void main(
	point PointLightVertexToGeometry input[1],
	inout TriangleStream<PointLightGeometryToPixel> output
)
{
    const float2 offset[4] =
    {
        { -1.0f, 1.0f },
        { 1.0f, 1.0f },
        { -1.0f, -1.0f },
        { 1.0f, -1.0f }
    };
    
    PointLightVertexToGeometry inputVertex = input[0];
    for (unsigned int index = 0; index < 4; index++)
    {
        PointLightGeometryToPixel vertex;
        vertex.myPosition = inputVertex.myPosition;
        vertex.myPosition.xy += offset[index] * pointLightPositionAndRange.w;
        vertex.myPosition = mul(pointLightToProjection, vertex.myPosition);
        vertex.myUV = vertex.myPosition.xyw;
        vertex.myUV.y *= -1.0f;
        output.Append(vertex);
    }
}