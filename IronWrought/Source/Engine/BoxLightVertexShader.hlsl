#include "BoxLightShaderStructs.hlsli"

BoxLightVertexToPixel main(BoxLightVertexInput input)
{   
    BoxLightVertexToPixel output;
    
    
    float4 vertexObjectPos = input.myPosition.xyzw;
    float4 vertexWorldPos = mul(toBoxLightWorld, vertexObjectPos);
    float4 vertexViewPos = mul(boxLightToCamera, vertexWorldPos);
    float4 vertexProjectionPos = mul(boxLightToProjection, vertexViewPos);
    
    
    //output.myPosition = input.myPosition;
    ////output.myPosition.z += boxLightDirection * boxLightPositionAndRange.w;
    ////output.myPosition.x += boxLightDirectionNormal1.xyz * boxLightWidthAndHeight.x;
    ////output.myPosition.y += boxLightDirectionNormal2.xyz * boxLightWidthAndHeight.y;
    //output.myPosition = mul(toBoxLightWorld, output.myPosition);
    //output.myWorldPosition = output.myPosition;
    //output.myPosition = mul(boxLightToCamera, output.myPosition);
    //output.myPosition = mul(boxLightToProjection, output.myPosition);
    
    output.myPosition = vertexProjectionPos;
    output.myClipSpacePosition = vertexProjectionPos.xyw;
    output.myWorldPosition = vertexWorldPos;
    
    output.myUV = output.myPosition.xyw;
    output.myUV.y *= -1.0f;
    
    return output;
}