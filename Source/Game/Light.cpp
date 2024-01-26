#pragma once
#include "pch.h"
#include "Light.h"
#include <Engine/PointLight.h>

void BaseLight::SetRange(const float aRange)
{
    myPointLight.SetRange(aRange);
}

void BaseLight::SetIntensity(const float anIntensity)
{
    myPointLight.SetIntensity(anIntensity);
    myStoredIntensity = anIntensity;
}

void BaseLight::SetPosition(const Vector3f& aPosition)
{
    myPointLight.SetPosition(aPosition);
}

void BaseLight::SetColor(const Vector3f& aColor)
{
    myPointLight.SetColor(aColor);
}

void BaseLight::SetAsFlicker(bool aIsFlickering)
{
    myIsFlickering = aIsFlickering;
}

#include <Engine/CDebugDrawer.h>
void BaseLight::Update(float aDeltaTime)
{
    if (myIsFlickering)
    {
        if (myIsGoingUp)
        {
            myCurrent += aDeltaTime * 4.0f;
            if (myCurrent >= myTarget)
            {
                myIsGoingUp = !myIsGoingUp;

                myTarget = (10.f - static_cast<float>(rand() % 4)) * 0.1f;
            }
        }
        else
        {
            myCurrent -= aDeltaTime * 3.5f;
            if (myCurrent <= myTarget)
            {
                myIsGoingUp =! myIsGoingUp;

                myTarget = (10.f + static_cast<float>(rand() % 4)) * 0.1f;
            }
        }
        /*myTime += static_cast<float>(rand() % 100) * 0.001f;
        float x = myTime * 0.5f;
        float r = sin(x * 2) + 1 + cos(x * 8);
        r *= 0.25f;
        r += 1.0f;*/
        myPointLight.SetIntensity(myCurrent * myStoredIntensity);
    }
    else
    {
        SetIntensity(myStoredIntensity);
    }
}
