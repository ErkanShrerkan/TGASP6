#pragma once
namespace SE
{
    class CSpotLight
    {
    public:
        void SetPosition(const Vector3f& aPosition);
        void SetColor(const Vector3f& aColor);
        void SetDirection(const Vector3f& aDirection);
        void SetRange(float aRange);
        void SetIntensity(float anIntensity);
        void SetInnerAngle(float anInnerAngle);
        void SetOuterAngle(float anOuterAngle);

        const Vector4f& GetBundledPosition() const noexcept;
        const Vector4f& GetBundledColor() const noexcept;
        const Vector4f& GetBundledDirection() const noexcept;
        const float GetInnerAngle() const noexcept;
        const float GetOuterAngle() const noexcept;
        Vector4f& GetRawBundledPosition();
        Vector4f& GetRawBundledColor();
        Vector4f& GetRawBundledDirection();
        float& GetRawInnerAngle();
        float& GetRawOuterAngle();

    private:
        // XYZ - Position
        // W - Range
        Vector4f myPosition;
        // RGB - Color
        // A - Intensity
        Vector4f myColor;
        // HACK: Can be Vector3f?
        Vector4f myDirection;

        float myInnerAngle;
        float myOuterAngle;
    };
}
