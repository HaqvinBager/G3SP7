#include "PointLightShaderStructs.hlsli"

PointLightVertexToGeometry main(PointLightVertexInput input)
{
    PointLightVertexToGeometry returnValue;
    float4 vertexObjectPos = input.myPosition.xyzw;
    float4 vertexWorldPos = mul(pointLightToWorld, vertexObjectPos);
    float4 vertexViewPos = mul(pointLightToCamera, vertexWorldPos);
    returnValue.myPosition = vertexViewPos;
    //returnValue.myRange = input.myRange;
    return returnValue;
}