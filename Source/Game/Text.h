#pragma once
#include <d3d11.h>
#include <CommonUtilities\Math\Matrix4x4.hpp>
struct Font;
class Text
{
public:
    struct TextData
    {
        UINT myNumberOfVertices;
        UINT myNumberOfIndices;
        UINT myStride;
        UINT myOffset;
        ID3D11Buffer* myVertexBuffer;
        ID3D11Buffer* myIndexBuffer;
        D3D_PRIMITIVE_TOPOLOGY myPrimitiveTopology;

    };
    void Render();
    void Init(TextData someTextData, std::wstring someText, Font* aFont);
    FORCEINLINE TextData GetTextData() { return myTextData; }
    FORCEINLINE const CommonUtilities::Matrix4x4<float>& GetTransform() const { return myTransform; }
    void SetTransform(const Matrix4x4f& aTransform);
    void SetRotation(const Vector3f& aRotation);
    void SetPosition(const Vector3f& aPosition);
    void SetScale(const Vector3f& aScale);

    FORCEINLINE Font* GetFont() { return myFont; }

private:
    TextData myTextData = {};
    std::wstring myText;
    Matrix4x4f myTransform{};
    Font* myFont;
};

