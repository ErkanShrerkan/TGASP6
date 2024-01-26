#pragma once
#include <string>
#include <CommonUtilities\Container\KeyedPool.hpp>

struct ID3D11VertexShader;
struct ID3D11Buffer;
struct ID3D11InputLayout;
namespace SE
{
    class CSprite;
    class CSpriteFactory
    {
    public:
        void InitFactory();
        SE::CSprite* GetSprite(const std::string& aSpritePath);

    private:
        SE::CSprite* CreateSprite(const std::string& aSpritePath);
        CommonUtilities::KeyedPool<std::string, CSprite> myPool;

    private:
        struct SVertex
        {
            float x, y, z, w;  // Position
            float r, g, b, a;  // Color
            float u, v;		   // UV Mapping
        };

        bool myHasInited = false;
        ID3D11VertexShader* myVertexShader;
        ID3D11Buffer* myIndexBuffer;
        ID3D11InputLayout* myInputLayout;
    };
}
