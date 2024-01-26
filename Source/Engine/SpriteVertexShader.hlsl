#include "Structs.hlsli"
#include "Functions.hlsli"

VertexToPixel_Sprite main(VertexInput_Sprite input)
{
    VertexToPixel_Sprite returnValue;
    
    float2 size = mySpriteSize * .5f;
    float rads = mySpriteRotation / 180 * PI;
    float2 pivot = mySpritePivot;
    float2 offset = float2(1 - mySpritePivot.x * 2, 1 - mySpritePivot.y * 2);
    
    float x = float(PPBD_resolution.x);
    float y = float(PPBD_resolution.y);
    float aspect = x / y;

    float2x2 rotation =
    {
        cos(rads), -sin(rads) * (1.0 / aspect),
        sin(rads) * (aspect), cos(rads),
    };
    
    float2 pos = input.myPosition.xy;
    pos *= size;
    pos = mul(rotation, pos);
    offset = mul(rotation, offset * size);
    pos += offset;
    pos += mySpritePosition.xy;
    pos = pos * 2 - 1;
    pos.y *= -1;
    
    returnValue.myPosition = float4(pos.x, pos.y, .5, 1);
    returnValue.myColor = input.myColor;
    
    returnValue.myRectUV = input.myUV;
    float2 uv = input.myUV;
    uv.x = Remap(uv.x, 0, 1, mySpriteRect.x, mySpriteRect.z);
    uv.y = Remap(uv.y, 0, 1, mySpriteRect.y, mySpriteRect.w);
    returnValue.myUV = uv;
    
    return returnValue;
}
