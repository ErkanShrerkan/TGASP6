#pragma once
#include <memory>
#include <d3d11.h>
namespace SE
{
    class CCamera;
}
class Text;
class TextRenderer
{
public:
    bool Init();
    void Render(SE::CCamera* aCamera, const std::vector<std::shared_ptr<Text>>& someText);

private:
    struct FrameBufferData
    {
        Matrix4x4f myToCamera;
        Matrix4x4f myToProjection;
        Vector4f myCameraPosition;
        float myNearPlane;
        float myFarPlane;
        int myRenderMode;
        float garbage;
    } myFrameBufferData;

    struct ObjectBufferData
    {
        Matrix4x4f myToWorld;
    }myObjectBufferData;

    ID3D11Buffer* myFrameBuffer;
    ID3D11Buffer* myObjectBuffer;

    ID3D11VertexShader* myVertexShader;
    ID3D11PixelShader* myPixelShader;
    ID3D11InputLayout* myInputLayout;
};

