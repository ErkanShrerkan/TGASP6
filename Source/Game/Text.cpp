#include "pch.h"
#include "Text.h"
#include <Engine\Engine.h>
#include<Engine\GraphicsEngine.h>
#include "Model.h"
void Text::Render()
{
    //SE::CEngine::GetInstance()->GetActiveScene()->AddInstance(*this);
}

void Text::Init(TextData someTextData, std::wstring someText, Font* aFont)
{
    myTextData = someTextData;
    myText = someText;
    myFont = aFont;
}

void Text::SetTransform(const Matrix4x4f& aTransform)
{
    myTransform = aTransform;
}

void Text::SetRotation(const Vector3f& aRotation)
{
    CommonUtilities::Matrix4x4<float> rotation(myTransform);
    rotation *= CommonUtilities::Matrix4x4<float>::CreateRotationAroundX(aRotation.x);
    rotation *= CommonUtilities::Matrix4x4<float>::CreateRotationAroundY(aRotation.y);
    rotation *= CommonUtilities::Matrix4x4<float>::CreateRotationAroundZ(aRotation.z);
    myTransform = rotation;
}

void Text::SetPosition(const Vector3f& aPosition)
{
    myTransform(4, 1) = aPosition.x;
    myTransform(4, 2) = aPosition.y;
    myTransform(4, 3) = aPosition.z;
}

void Text::SetScale(const Vector3f& aScale)
{
    myTransform(1, 1) *= aScale.x;
    myTransform(2, 2) *= aScale.y;
    myTransform(3, 3) *= aScale.z;
    //myTransform(4, 4) = 1;
}
