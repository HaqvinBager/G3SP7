#include "BoxLightShaderStructs.hlsli"

BoxLightVertexToPixel main(BoxLightVertexInput input)
{   
    BoxLightVertexToPixel output;
    output.myPosition = input.myPosition;
    //output.myPosition.xyz *= boxLightPositionAndRange.w;
    output.myPosition = mul(toBoxLightWorld, output.myPosition);
    output.myWorldPosition = output.myPosition;
    output.myPosition = mul(boxLightToCamera, output.myPosition);
    output.myPosition = mul(boxLightToProjection, output.myPosition);
    output.myUV = output.myPosition.xyw;
    output.myUV.y *= -1.0f;
    
    return output;
}