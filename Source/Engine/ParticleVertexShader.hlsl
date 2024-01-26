#include "Structs.hlsli"

VertexToGeometry_Particle main(VertexInput_Particle input)
{
    VertexToGeometry_Particle output;
    
    float4x4 identity =
    {
        { 1, 0, 0, 0 },
        { 0, 1, 0, 0 },
        { 0, 0, 1, 0 },
        { 0, 0, 0, 1 }
    };
    
    float4 vertexObjectPos = input.myPosition;
    float4 vertexWorldPos = mul(identity, vertexObjectPos);
    float4 vertexViewPos = mul(FBD_toCamera, vertexWorldPos);
    
    output.myPosition = vertexViewPos;
    output.myColor = float4(input.myColor.rgb * input.myEmissiveStrength, input.myColor.a);
    output.mySize = input.mySize;
    output.myHorizontalOffset = input.myHorizontalOffset;
    output.myEmissiveStrength = input.myEmissiveStrength;
    output.myProjection = FBD_toProjection;
    
    return output;
}
