#include "PointLightShaderStructs.hlsli"

/*PointLightVertexToGeometry*/
PointLightVertexToPixel main(PointLightVertexInput input)
{
    // For Geometry Shader
    //PointLightVertexToGeometry returnValue;
    //float4 vertexObjectPos = input.myPosition.xyzw;
    //float4 vertexWorldPos = mul(pointLightToWorld, vertexObjectPos);
    //float4 vertexViewPos = mul(pointLightToCamera, vertexWorldPos);
    //returnValue.myPosition = vertexViewPos;
    ////returnValue.myRange = input.myRange;
    //return returnValue;
    
    PointLightVertexToPixel output;
    output.myPosition = input.myPosition;
    output.myPosition.xyz *= pointLightPositionAndRange.w;
    output.myPosition = mul(pointLightToWorld, output.myPosition);
    output.myWorldPosition = output.myPosition;
    output.myPosition = mul(pointLightToCamera, output.myPosition);
    output.myPosition = mul(pointLightToProjection, output.myPosition);
    output.myUV = output.myPosition.xyw;
    output.myUV.y *= -1.0f;
    
    return output;
}