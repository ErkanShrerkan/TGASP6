#include "pch.h"
#include "SpotLight.h"

namespace SE
{
    void CSpotLight::SetPosition(const Vector3f& aPosition)
    {
        myPosition.x = aPosition.x;
        myPosition.y = aPosition.y;
        myPosition.z = aPosition.z;
    }
    void CSpotLight::SetColor(const Vector3f& aColor)
    {
        myColor.r = aColor.r;
        myColor.g = aColor.g;
        myColor.b = aColor.b;
    }
    void CSpotLight::SetDirection(const Vector3f& aDirection)
    {
        myDirection.x = aDirection.x;
        myDirection.y = aDirection.y;
        myDirection.z = aDirection.z;
    }
    void CSpotLight::SetRange(float aRange)
    {
        myPosition.w = aRange;
    }
    void CSpotLight::SetInnerAngle(float anInnerAngle)
    {
        myInnerAngle = anInnerAngle;
    }
    void CSpotLight::SetOuterAngle(float anOuterAngle)
    {
        myOuterAngle = anOuterAngle;
    }
    void CSpotLight::SetIntensity(float anIntensity)
    {
        myColor.a = anIntensity;
    }
    const Vector4f& CSpotLight::GetBundledPosition() const noexcept
    {
        return myPosition;
    }
    const Vector4f& CSpotLight::GetBundledColor() const noexcept
    {
        return myColor;
    }
    const Vector4f& CSpotLight::GetBundledDirection() const noexcept
    {
        return myDirection;
    }
    const float CSpotLight::GetInnerAngle() const noexcept
    {
        return myInnerAngle;
    }
    const float CSpotLight::GetOuterAngle() const noexcept
    {
        return myOuterAngle;
    }
    Vector4f& CSpotLight::GetRawBundledPosition()
    {
        return myPosition;
    }
    Vector4f& CSpotLight::GetRawBundledColor()
    {
        return myColor;
    }
    Vector4f& CSpotLight::GetRawBundledDirection()
    {
        return myDirection;
    }
    float& CSpotLight::GetRawInnerAngle()
    {
        return myInnerAngle;
    }
    float& CSpotLight::GetRawOuterAngle()
    {
        return myOuterAngle;
    }
}
