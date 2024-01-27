#pragma once
#include <array>
struct ID3D11VertexShader;
struct ID3D11PixelShader;
namespace SE
{
    class CFullscreenRenderer
    {
    public:
        enum EShader : uint
        {
            EShader_Copy,
            EShader_Luminance,
            EShader_GaussianHorizontal,
            EShader_GaussianVertical,
            EShader_Bloom,
            EShader_Border,
            EShader_ToCamera,
            EShader_WaterUniversal,
            EShader_WaterMerge,
            EShader_DebugSpheres,
            EShader_Fog,
            EShader_AlphaBlend,
            EShader_AdditiveBlend,
            EShader_Outline,
            EShader_Highlight,


            // Keep last, don't remove
            EShader_Count
        };

        bool Init();
        void Render(const EShader& aMode);
        ID3D11VertexShader* GetVertexShader() { return myVertexShader; }
        ID3D11PixelShader* GetPixelShader(EShader aShader) { return myPixelShaders[aShader]; }
        ~CFullscreenRenderer();

    private:
        struct SFullscreenDataBuffer
        {
            float2 myResolution;
            float myElapsedTime;
        }
        myDataBuffer;

        ID3D11VertexShader* myVertexShader;
        std::array<ID3D11PixelShader*, EShader_Count> myPixelShaders;
    };
}
