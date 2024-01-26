#pragma once

namespace SE
{
    class CPointLight
    {
    public:
        void SetPosition(const Vector3f& aPosition);
        void SetColor(const Vector3f& aColor);
        void SetRange(float aRange);
        void SetIntensity(float anIntensity);

        const Vector4f& GetBundledPosition() const noexcept;
        const Vector4f& GetBundledColor() const noexcept;
        Vector4f& GetRawBundledPosition();
        Vector4f& GetRawBundledColor();

    private:
        // XYZ - Position
        // W - Range
        Vector4f myPosition;
        // RGB - Color
        // A - Intensity
        Vector4f myColor;
    };
}
