#pragma once
#include "AutoReleaser.h"
#include "Camera.h"
#include "FullscreenTexture.h"
#include <memory>

struct ID3D11ShaderResourceView;
class CLightFactory;
namespace SE
{
    class CEnvironmentLight
    {
        friend CLightFactory;
    public:
        CEnvironmentLight();
        ~CEnvironmentLight();

        void SetDirection(const Vector3f& aDirection);
        void SetRotation(const Vector3f& aRotation);
        void SetDirectionalColor(const Vector4f& aColor);
        void SetAmbientColor(const Vector4f& aColor);
        ID3D11ShaderResourceView* const& GetCubeMap() const;

        const Vector4f& GetDirectionalColor() const;
        const Vector4f& GetAmbientColor() const;
        const Vector4f& GetDirection() const;
        const float& GetMipLevels() const;
        Vector4f& GetRawDirection();
        Vector4f& GetRawAmbient();
        Vector4f& GetRawDirectionalColor();
        std::shared_ptr<CCamera>& GetShadowCam() { return myShadowCam; }
        std::vector<CFullscreenTexture>& GetShadowTextures() { return myShadowTextures; }
        const std::vector<Matrix4x4f>& GetShadowCamProjections() { return myCamProjections; }

    private:
        void AddProjection(Matrix4x4f aProj);

        // TODO: Make this not yours
        ID3D11ShaderResourceView* myCubemap;
        float myMipLevels = .0f;
        Vector4f myDirectionalColor;
        Vector4f myAmbientColor;
        Vector4f myDirection;
        Matrix4x4f myTransform;
        std::shared_ptr<CCamera> myShadowCam;
        std::vector<CFullscreenTexture> myShadowTextures;
        std::vector<Matrix4x4f> myCamProjections;
    };
}
