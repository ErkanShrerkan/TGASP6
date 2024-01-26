Texture2D mySpriteTexture : register(t0);
Texture2D myMaskTexture : register(t1);
SamplerState myDefaultSampler : register(s0);

struct VertexInput_Sprite
{
    float4 myPosition : POSITION;
    float4 myColor : COLOR;
    float2 myUV : UV;
};

struct VertexToPixel_Sprite
{
    float4 myPosition : SV_POSITION;
    float4 myColor : COLOR;
    float2 myUV : UV;
};

struct PixelOutput_Sprite
{
    float4 myColor : SV_TARGET;
};
