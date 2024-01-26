#include "ShaderStructs.hlsli"

VertexToPixel main(VertexInput input)
{
    VertexToPixel returnValue;
    returnValue.myPosition = input.myPosition;
    returnValue.myColor = input.myColor;
	return returnValue;
}
