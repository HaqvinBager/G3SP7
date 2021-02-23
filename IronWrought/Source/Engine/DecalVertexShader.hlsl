#include "DecalShaderStructs.hlsli"

VertexToPixel main(VertexInput input)
{
    VertexToPixel returnValue;
    
    float4 vertexObjectPos = input.myPosition.xyzw;
    float4 vertexWorldPos = mul(toWorld, vertexObjectPos);
    float4 vertexViewPos = mul(toCameraSpace, vertexWorldPos);
    float4 vertexProjectionPos = mul(toProjectionSpace, vertexViewPos);
    
    float3x3 toWorldRotation = (float3x3) toWorld;
    float3 vertexWorldNormal = mul(toWorldRotation, input.myNormal.xyz);
    float3 vertexWorldTangent = mul(toWorldRotation, input.myTangent.xyz);
    float3 vertexWorldBitangent = mul(toWorldRotation, input.myBitangent.xyz);
    
    returnValue.myPosition = vertexProjectionPos;
    returnValue.myClipSpacePosition = vertexProjectionPos.xyw;
    returnValue.myNormal = float4(vertexWorldNormal, 0);
    returnValue.myTangent = float4(vertexWorldTangent, 0);
    returnValue.myBitangent = float4(vertexWorldBitangent, 0);

    returnValue.myUV = input.myUV;
     
    return returnValue;
}