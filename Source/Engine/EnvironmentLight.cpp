#include "pch.h"
#include "EnvironmentLight.h"
#include "Engine\Engine.h"
#include "Engine\ContentLoader.h"
#include "DX11.h"

#include "Engine\CDebugDrawer.h"

namespace SE
{
    CEnvironmentLight::CEnvironmentLight()
    {
        unsigned y = 256;
        myShadowTextures.resize(2);
        for (int i = 0; i < 2; i++)
        {
            myShadowTextures[i] = CEngine::GetInstance()->GetContentLoader()->GetTextureFactory().CreateFullscreenTexture({ y * 2, y * 2 }, DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT);
        }
    }
    CEnvironmentLight::~CEnvironmentLight()
    {
    }
    void CEnvironmentLight::SetDirection(const Vector3f& aDirection)
    {
        myDirection = Vector4f(aDirection, 0.f);
    }
    void CEnvironmentLight::SetRotation(const Vector3f& aRotation)
    {
        //float4 pos(myTransform(4, 1), myTransform(4, 2), myTransform(4, 3), 1);

        Matrix4x4f identity;

        identity *= Matrix4x4f::CreateRotationAroundX(Math::DegreeToRadian(aRotation.x));
        identity *= Matrix4x4f::CreateRotationAroundY(Math::DegreeToRadian(aRotation.y));
        identity *= Matrix4x4f::CreateRotationAroundZ(Math::DegreeToRadian(aRotation.z));

        //identity(4, 1) = pos.x;
        //identity(4, 2) = pos.y;
        //identity(4, 3) = pos.z;

        myTransform.SetRotation(identity);

        float3 dir = { myTransform(3, 1), myTransform(3, 2), myTransform(3, 3) };
        myDirection = { dir.x, dir.y, dir.z, 0 };

        dir *= -10000.f;
        myShadowCam->SetRotation(aRotation);
        myShadowCam->SetPosition(dir);
    }
    void CEnvironmentLight::SetDirectionalColor(const Vector4f& aColor)
    {
        myDirectionalColor = aColor;
    }
    void CEnvironmentLight::SetAmbientColor(const Vector4f& aColor)
    {
        myAmbientColor = aColor;
    }
    ID3D11ShaderResourceView* const& CEnvironmentLight::GetCubeMap() const
    {
        return myCubemap;
    }
    Vector4f& CEnvironmentLight::GetRawDirectionalColor()
    {
        return myDirectionalColor;
    }
    void CEnvironmentLight::AddProjection(Matrix4x4f aProj)
    {
        myCamProjections.push_back(aProj);
    }
    const Vector4f& CEnvironmentLight::GetDirectionalColor() const
    {
        return myDirectionalColor;
    }
    const Vector4f& CEnvironmentLight::GetAmbientColor() const
    {
        return myAmbientColor;
    }
    const Vector4f& CEnvironmentLight::GetDirection() const
    {
        return myDirection;
    }
    const float& CEnvironmentLight::GetMipLevels() const
    {
        return myMipLevels;
    }
    Vector4f& CEnvironmentLight::GetRawDirection()
    {
        return myDirection;
    }
    Vector4f& CEnvironmentLight::GetRawAmbient()
    {
        return myAmbientColor;
    }
}
