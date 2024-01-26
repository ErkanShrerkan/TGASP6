struct VertexInput
{
    float4 myPosition : POSITION;
    float4 myColor : COLOR;
    float2 myUV : UV;
};

struct VertexToPixel
{
    float4 myPosition : SV_POSITION;
    float4 myColor : COLOR;
    float2 myUV : UV;
};

struct PixelOutput
{
    float4 myColor : SV_TARGET;
};
