#include "SpriteShaderStructs.hlsli"

[maxvertexcount(4)]
void main(
	point VertexToGeometry input[1],
	inout TriangleStream<GeometryToPixel> output
)
{
    const float2 offset[4] =
    {
        { -1.0f, 1.0f },
        { 1.0f, 1.0f },
        { -1.0f, -1.0f },
        { 1.0f, -1.0f }
    };

    VertexToGeometry inputVertex = input[0];
    
    const float2 uv[4] =
    {
        { inputVertex.myUVRect.x, inputVertex.myUVRect.y },
        { inputVertex.myUVRect.z, inputVertex.myUVRect.y },
        { inputVertex.myUVRect.x, inputVertex.myUVRect.w },
        { inputVertex.myUVRect.z, inputVertex.myUVRect.w }
    };
    
    float2 aspectRatioDivider = { 9.0f / 16.0f, 1.0f };
    for (unsigned int index = 0; index < 4; index++)
    {
        GeometryToPixel vertex;
        vertex.myPosition = 0.0f;
        vertex.myPosition += inputVertex.myPosition;
        vertex.myPosition.xy += offset[index] * inputVertex.mySize;
        float2 intermediate = vertex.myPosition.xy;
        vertex.myPosition.x = intermediate.x * cos(inputVertex.myRotation) - intermediate.y * sin(inputVertex.myRotation);
        vertex.myPosition.y = intermediate.x * sin(inputVertex.myRotation) + intermediate.y * cos(inputVertex.myRotation);
        vertex.myPosition.xy *= aspectRatioDivider;
        vertex.myColor = inputVertex.myColor;
        vertex.myUV = uv[index];
        output.Append(vertex);
    }
}