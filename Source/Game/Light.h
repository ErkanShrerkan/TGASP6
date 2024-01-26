#include <CommonUtilities/MathBundle.hpp>
#include <Engine/PointLight.h>

// Assume all light are pointlights for now
// 11/11/2021
class BaseLight
{
    friend class LightSystem;
private:
    SE::CPointLight myPointLight;
    bool myIsFlickering = false;
    float myStoredIntensity = 0.0f;
    float myCurrent = 1.0f;
    float myTarget = 0.0f;
    bool myIsGoingUp = true;

public:
    void SetRange(const float aRange);
    void SetIntensity(const float anIntensity);
    void SetPosition(const Vector3f& aPosition);
    void SetColor(const Vector3f& aColor);

public:
    void SetAsFlicker(bool aIsFlickering = true);
    void Update(float aDeltaTime);
};
typedef BaseLight* Light;
