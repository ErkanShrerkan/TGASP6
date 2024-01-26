#include "Structs.hlsli"

PixelOutput_GBuffer main(VertexToPixel_GBuffer input)
{
    PixelOutput_GBuffer returnValue;
    
    if (objAlbedoTexture.Sample(defaultSampler, input.myUV).a < PPBD_alphaThreshold / 100)
    {
        discard;
    }
    
    returnValue.myColor.r = input.myDepth;
    returnValue.myColor.g = input.myDepth;
    returnValue.myColor.b = input.myDepth;
    returnValue.myColor.a = 1;
    return returnValue;
}