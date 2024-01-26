#pragma once
#include "Engine/Component.h"

namespace SE
{
	class CGameObject;
	class CCamera;
	class CCameraFactory;
}

class CameraComponent : public SE::CComponent
{
public:
	CameraComponent(SE::CGameObject&);
	~CameraComponent();

	void Init() override;
	void Update() override;
	void SetFollowObject(SE::CGameObject* anObjectToFollow);

	void SetRotation(const Vector3f& aRotation);
	void LerpFollow(const Vector3f& aPosition);
private:

	SE::CGameObject* myObjectToFollow = nullptr;

	SE::CCamera* myCCamera = nullptr;

};

