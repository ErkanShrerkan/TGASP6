#include "Structs.hlsli"

[maxvertexcount(4)]
void main(
	point VertexToGeometry_Particle input[1],
	inout TriangleStream<GeometryToPixel_Particle> output
)
{
    VertexToGeometry_Particle inputVertex = input[0];
    
    const float2 offset[4] =
    {
        { -1,  2 },
        {  1,  2 },
        { -1,  0 },
        {  1,  0 }
    };
    
    const float uvSize = inputVertex.mySize.y;
    const float index = inputVertex.myHorizontalOffset;
    const float2 uvs[4] =
    {
        { (index + 0.0) * uvSize, 0.0 },
        { (index + 1.0) * uvSize, 0.0 },
        { (index + 0.0) * uvSize, 1.0 },
        { (index + 1.0) * uvSize, 1.0 }
    };
    
    for (unsigned int i = 0; i < 4; i++)
    {
        GeometryToPixel_Particle vertex;
        
        vertex.myPosition = inputVertex.myPosition;
        vertex.myPosition.xy += offset[i] * (float2)inputVertex.mySize.x;
        vertex.myPosition = mul(inputVertex.myProjection, vertex.myPosition);
        vertex.myColor = inputVertex.myColor;
        vertex.myUV = uvs[i];
        vertex.myEmissiveStrength = inputVertex.myEmissiveStrength;
        output.Append(vertex);
    }
}
