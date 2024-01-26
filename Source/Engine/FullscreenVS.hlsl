#include "Structs.hlsli"

VertexToPixel_Fullscreen main(VertexInput_Fullscreen input)
{
	float4 pos[3] =
    {
        float4(-1.0, -1.0, 0.0, 1.0),
        float4(-1.0,  3.0, 0.0, 1.0),
        float4( 3.0, -1.0, 0.0, 1.0)
    };
    
    double2 uv[3] =
    {
        double2(0.0,  1.0),
        double2(0.0, -1.0),
        double2(2.0,  1.0)
    };
	
    VertexToPixel_Fullscreen returnValue;
    returnValue.myPosition = pos[input.myIndex];
    returnValue.myUV = uv[input.myIndex];
	return returnValue;
}
