#include "pch.h"
#include "PointLight.h"

namespace SE
{
    void CPointLight::SetPosition(const Vector3f& aPosition)
    {
        myPosition.x = aPosition.x;
        myPosition.y = aPosition.y;
        myPosition.z = aPosition.z;
    }
    void CPointLight::SetColor(const Vector3f& aColor)
    {
        myColor.r = aColor.r;
        myColor.g = aColor.g;
        myColor.b = aColor.b;
    }
    void CPointLight::SetRange(float aRange)
    {
        myPosition.w = aRange;
    }
    void CPointLight::SetIntensity(float anIntensity)
    {
        myColor.a = anIntensity;
    }
    const Vector4f& CPointLight::GetBundledPosition() const noexcept
    {
        return myPosition;
    }
    const Vector4f& CPointLight::GetBundledColor() const noexcept
    {
        return myColor;
    }
    Vector4f& CPointLight::GetRawBundledPosition()
    {
        return myPosition;
    }
    Vector4f& CPointLight::GetRawBundledColor()
    {
        return myColor;
    }
}
