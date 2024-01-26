#include "pch.h"
#include "CameraComponent.h"
#include "Engine/GameObject.h"
#include "Engine/Camera.h"
#include "Engine/CameraFactory.h"

CameraComponent::CameraComponent(SE::CGameObject& aParent)
	: SE::CComponent(aParent)
{
	//myCCamera = SE::CCameraFactory::CreateCamera()
}

CameraComponent::~CameraComponent()
{
}

void CameraComponent::Init()
{
}

void CameraComponent::Update()
{
	if (myObjectToFollow != nullptr)
	{
		/*Vector3f pos;
		pos.x = myObjectToFollow->Position.x;
		pos.y = GetParent().Position.y;
		pos.z = myObjectToFollow->Position.x;
		GetParent()->SetPosition(pos);*/
	}
}

void CameraComponent::SetFollowObject(SE::CGameObject* anObjectToFollow)
{
	anObjectToFollow;
	//set an object to follow
}

void CameraComponent::SetRotation(const Vector3f& aRotation)
{
	aRotation;
	//Set from what angle you see the player
}

void CameraComponent::LerpFollow(const Vector3f& aPosition)
{
	aPosition;
	//move to position with lerp
}
