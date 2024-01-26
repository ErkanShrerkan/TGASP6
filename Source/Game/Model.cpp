#include "pch.h"
#include "Model.h"
#include <Engine/ModelInstance.h>
#include <Engine/DX11.h>
#include <Engine/ContentLoader.h>
#include <Engine/Engine.h>
#include <Engine/Scene.h>

Model::Model()
    : myEngineScene(nullptr)
{}

Model::Model(const std::string& aModelPath)
{
    SE::CModel* model = SE::DX11::Content->GetModelFactory().GetModel(aModelPath);
    myEngineModelInstance.Init(model);
}

void Model::SetTransform(const Vector3f& aPosition, const Vector3f& aRotation)
{
    myEngineModelInstance.SetTransform(aPosition, aRotation);
}

void Model::SetTransform(const Matrix4x4f& aTransform)
{
    myEngineModelInstance.SetTransform(aTransform);
}

Matrix4x4f& Model::GetTransform()
{
    return myEngineModelInstance.GetTransform();
}

void Model::SetRotation(const Vector3f& aRotation)
{
    myEngineModelInstance.SetRotation(aRotation);
}

void Model::SetPosition(const Vector3f& aPosition)
{
    myEngineModelInstance.SetPosition(aPosition);
}

const Vector3f Model::GetPosition() const
{
    return myEngineModelInstance.GetPosition();
}

void Model::Move(const Vector3f& aMovement)
{
    myEngineModelInstance.Move(aMovement);
}

void Model::Rotate(const Vector3f& aRotation)
{
    myEngineModelInstance.Rotate(aRotation);
}

const Vector3f& Model::GetScale() const noexcept
{
    return myEngineModelInstance.GetScale();
}

void Model::SetScale(const Vector3f& aScale)
{
    myEngineModelInstance.SetScale(aScale);
}

void Model::Render()
{
    myEngineModelInstance.Render();
}
