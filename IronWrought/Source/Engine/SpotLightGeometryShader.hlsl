#include "SpotLightShaderStructs.hlsli"

[maxvertexcount(5)]
void main(
	point SpotLightVertexToGeometry input[1],
	inout TriangleStream<SpotLightGeometryToPixel> output
)
{
    const float2 offset[4] =
    {
        { -1.0f, 1.0f },
        { 1.0f, 1.0f },
        { -1.0f, -1.0f },
        { 1.0f, -1.0f }
    };
    
    SpotLightVertexToGeometry inputVertex = input[0];
    SpotLightGeometryToPixel vertex;
    vertex.myPosition = inputVertex.myPosition;
    vertex.myPosition = mul(spotLightToProjection, vertex.myPosition);
    vertex.myUV = vertex.myPosition.xyw;
    vertex.myUV.y *= -1.0f;
    output.Append(vertex);
    
    for (unsigned int index = 0; index < 4; index++)
    {
        SpotLightGeometryToPixel vertex;
        vertex.myPosition = inputVertex.myPosition;
        vertex.myPosition.xy += offset[index] * (1.0f / spotLightDirectionAndAngleExponent.w) /*spotLightPositionAndRange.w*/;
        vertex.myPosition = mul(spotLightToProjection, vertex.myPosition);
        vertex.myUV = vertex.myPosition.xyw;
        vertex.myUV.y *= -1.0f;
        output.Append(vertex);
    }
}