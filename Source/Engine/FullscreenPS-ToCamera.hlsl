#include "Structs.hlsli"

PixelOutput_Fullscreen main(VertexToPixel_Fullscreen input)
{
    PixelOutput_Fullscreen output;

    //float3 toEye = normalize(cameraPosition.xyz - input.myWorldPosition.xyz);
    //float3 normal = PixelShader_Normal(input).myColor.xyz;
    
    //output.myColor.rgb = dot(toEye, normal);
    output.myColor.rgb = 1;
    output.myColor.a = 1.0f;

    return output;
}