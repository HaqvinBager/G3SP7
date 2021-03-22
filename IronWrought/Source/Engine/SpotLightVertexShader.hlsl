#include "SpotLightShaderStructs.hlsli"

SpotLightVertexToGeometry main(SpotLightVertexInput input)
{
    SpotLightVertexToGeometry returnValue;
    float4 vertexObjectPos = input.myPosition.xyzw;
    //float4 vertexWorldPos = mul(spotLightToWorld, vertexObjectPos);
    //float4 vertexViewPos = mul(spotLightToCamera, vertexWorldPos);
    returnValue.myPosition = /*vertexViewPos*/input.myPosition.xyzw;

    return returnValue;
}