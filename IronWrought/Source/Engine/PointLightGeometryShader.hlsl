#include "PointLightShaderStructs.hlsli"

#define NUMBER_OF_VERTICES 18

[maxvertexcount(NUMBER_OF_VERTICES)]
void main(
	point PointLightVertexToGeometry input[1],
	inout TriangleStream<PointLightGeometryToPixel> output
)
{
    const float3 offset[NUMBER_OF_VERTICES] =
    {  
        { -1.0f, 1.0f, -1.0f },
        { 1.0f, 1.0f, -1.0f },
        
        { -1.0f, -1.0f, -1.0f },
        { 1.0f, -1.0f, -1.0f },
        
        //
        { -1.0f, -1.0f, 1.0f },
        { 1.0f, -1.0f, 1.0f },
        
        { -1.0f, 1.0f, 1.0f },
        { 1.0f, 1.0f, 1.0f },
        
        //
        { -1.0f, 1.0f, -1.0f },
        { 1.0f, 1.0f, -1.0f },
        
        
        
        { 1.0f, 1.0f, 1.0f },
        { 1.0f, -1.0f, -1.0f },
        
        //
        { 1.0f, -1.0f, 1.0f },
        { -1.0f, -1.0f, -1.0f },
        
        //
        { -1.0f, -1.0f, 1.0f },
        { -1.0f, 1.0f, -1.0f },
        
        { -1.0f, 1.0f, 1.0f },
        { -1.0f, -1.0f, 1.0f }
       
        };
    
    PointLightVertexToGeometry inputVertex = input[0];
    for (unsigned int index = 0; index < NUMBER_OF_VERTICES; index++)
    {
        PointLightGeometryToPixel vertex;
        vertex.myPosition = float4(0.0f, 0.0f, 0.0f, 1.0f) /*inputVertex.myPosition*/;
        vertex.myPosition.xyz += offset[index] * pointLightPositionAndRange.w;
        vertex.myPosition = mul(pointLightToWorld, vertex.myPosition);
        vertex.myPosition = mul(pointLightToCamera, vertex.myPosition);
        vertex.myPosition = mul(pointLightToProjection, vertex.myPosition);
        vertex.myUV = vertex.myPosition.xyw;
        vertex.myUV.y *= -1.0f;
        output.Append(vertex);
    }
}