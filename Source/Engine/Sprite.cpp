#include "pch.h"
#include "Sprite.h"
#include "Texture.h"
#include <Engine/DX11.h>
#include "Engine.h"
#include "Scene.h"
#include <Engine\ContentLoader.h>

namespace SE
{
    void CSprite::SetPosition(const Vector2f& aPosition)
    {
        myPosition.x = aPosition.x;
        myPosition.y = aPosition.y;
    }
    void CSprite::SetSize(const Vector2f& aSize)
    {
        mySize = aSize;
    }
    void CSprite::SetSizeRelativeToScreen(const Vector2f& aSize)
    {
        SetSize(aSize);
    }
    void CSprite::SetSizeRelativeToImage(const Vector2f& aSize)
    {
        mySize = GetNormalizedImageSize();
        mySize.x *= aSize.x;
        mySize.y *= aSize.y;
    }
    void CSprite::SetSizeRelativeToAnotherImage(const float2& aSize, CSprite& aSprite)
    {
        mySize = aSprite.GetNormalizedImageSize();
        mySize.x *= aSize.x;
        mySize.y *= aSize.y;
    }
    void CSprite::SetSizeRelativeToAnotherImage(const float2& aSize, CSprite* aSprite)
    {
        mySize = aSprite->GetNormalizedImageSize();
        mySize.x *= aSize.x;
        mySize.y *= aSize.y;
    }
    void CSprite::SetColor(const Vector4f& aColor)
    {
        myColor = aColor;
    }
    void CSprite::SetPivot(const Vector2f& aPivot)
    {
        myPivot = aPivot;
    }
    void CSprite::SetRotation(const float& aRotation)
    {
        myRotation = aRotation;
    }
    void CSprite::SetMask(const char* aFilePath)
    {
        myMaskTexture = CEngine::GetInstance()->GetContentLoader()->Load<CTexture>(aFilePath);
    }
    void CSprite::SetRect(const float4 aRect)
    {
        myRect = aRect;
    }
    void CSprite::SetIsGamma(bool aIsGamma)
    {
        myIsGamma = aIsGamma;
    }
    void CSprite::Render()
    {
        CEngine::GetInstance()->GetActiveScene()->AddInstance(this);
    }
    void CSprite::SetShaderData(float someData)
    {
        myData = someData;
    }
    Vector2f& CSprite::GetPosition()
    {
        return myPosition;
    }
    Vector2f CSprite::GetTopLeftPosition()
    {
        return myPosition - (GetNormalizedImageSize() / 2.f);
        
    }
    Vector2f& CSprite::GetSize()
    {
        return mySize;
    }
    Vector2f& CSprite::GetPivot()
    {
        return myPivot;
    }
    Vector4f& CSprite::GetColor()
    {
        return myColor;
    }
    Vector2f CSprite::GetImageSize()
    {
        return Vector2f(myTexture->GetWidth(), myTexture->GetHeight());
    }
    Vector2f CSprite::GetNormalizedImageSize()
    {
        float x = GetImageSize().x / 1920;
        float y = GetImageSize().y / 1080;
        return { x, y };
    }
    float4 CSprite::GetRect()
    {
        return myRect;
    }
    float CSprite::GetRotation()
    {
        return myRotation;
    }
    bool CSprite::GetIsGamma()
    {
        return myIsGamma;
    }
}
