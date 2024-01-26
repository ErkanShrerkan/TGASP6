#pragma once
#include <string_view>
#include <Engine/ModelInstance.h>

namespace SE
{
	class CScene;
}

class Model
{
public:
	Model();
    Model(const std::string& aModelPath);

	void SetTransform(const Vector3f& aPosition, const Vector3f& aRotation);
	void SetTransform(const Matrix4x4f& aTransform);
	Matrix4x4f& GetTransform();
	void SetRotation(const Vector3f& aRotation);
	void SetPosition(const Vector3f& aPosition);
	const Vector3f GetPosition() const;
	void Move(const Vector3f& aMovement);
	void Rotate(const Vector3f& aRotation);
	const Vector3f& GetScale() const noexcept;
	void SetScale(const Vector3f& aScale);

	void Render();

private:
	SE::CScene* myEngineScene;
    SE::CModelInstance myEngineModelInstance;
};
