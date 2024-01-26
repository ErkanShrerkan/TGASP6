#include "pch.h"
#include "ContentLoader.h"

namespace SE
{
    CFullscreenTexture CContentLoader::Load(const Vector2ui& aSize, DXGI_FORMAT aFormat)
    {
        return myTextureFactory.CreateFullscreenTexture(aSize, aFormat);
    }
    CFullscreenTexture CContentLoader::Load(ID3D11Texture2D* aTexture)
    {
        return myTextureFactory.CreateFullscreenTexture(aTexture);
    }
    SE::CModelFactory& CContentLoader::GetModelFactory()
    {
        return myModelFactory;
    }
    SE::CCameraFactory& CContentLoader::GetCameraFactory()
    {
        return myCameraFactory;
    }
    SE::CLightFactory& CContentLoader::GetLightFactory()
    {
        return myLightFactory;
    }
    SE::CTextureFactory& CContentLoader::GetTextureFactory()
    {
        return myTextureFactory;
    }
    SE::CSpriteFactory& CContentLoader::GetSpriteFactory()
    {
        return mySpriteFactory;
    }
    SE::CNavMeshLoader& CContentLoader::GetNavMeshLoader()
    {
        return myNavMeshLoader;
    }
    SE::CParticleFactory& CContentLoader::GetParticleFactory()
    {
        return myParticleFactory;
    }
}
