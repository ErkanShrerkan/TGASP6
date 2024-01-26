#pragma once
#pragma comment(lib, "d3d11.lib")
#include <dxgiformat.h>
#include <string>
#include <CommonUtilities\Container\KeyedPool.hpp>
#include "FullscreenTexture.h"
#include "GBuffer.h"

struct ID3D11Texture2D;
namespace SE
{
    class CTexture;
    class CTextureFactory
    {
    public:
        CFullscreenTexture CreateFullscreenTexture(const Vector2ui& aSize, DXGI_FORMAT aFormat);
        CFullscreenTexture CreateFullscreenDepth(const Vector2ui& aSize, DXGI_FORMAT aFormat);
        CFullscreenTexture CreateFullscreenTexture(ID3D11Texture2D* const& aTexture);
        CGBuffer CreateGBuffer(const Vector2ui& aSize);
        CTexture* LoadTexture(const std::string& aPath);
        CTexture* CreateTexture(const std::string& aPath);

    private:

        CommonUtilities::KeyedPool<std::string, CTexture> myPool;
    };
}
