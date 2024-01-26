#include "pch.h"
#include "ShadowRenderer.h"
#include "ModelInstance.h"
#include "Model.h"
#include "Camera.h"
#include <fstream>
#include <Engine\Engine.h>
#include <Engine\Animator.h>
#include "ShaderHelper.h"

namespace SE
{
	CShadowRenderer::CShadowRenderer()
	{
	}

	CShadowRenderer::~CShadowRenderer()
	{
	}

	bool CShadowRenderer::Init()
	{
		HRESULT result;
		D3D11_BUFFER_DESC bufferDesc = { 0 };
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		auto device = CEngine::GetInstance()->GetDXDevice();

		bufferDesc.ByteWidth = sizeof(ObjectBufferData);
		result = device->CreateBuffer(&bufferDesc, nullptr, &myObjectBuffer);
		if (FAILED(result))
		{
			return false;
		}

		bufferDesc.ByteWidth = sizeof(FrameBufferData);
		result = device->CreateBuffer(&bufferDesc, nullptr, &myFrameBuffer);
		if (FAILED(result))
		{
			return false;
		}

		result = device->CreateBuffer(&bufferDesc, nullptr, &myShadowFrameBuffer);
		if (FAILED(result))
		{
			return false;
		}

		if (!Helper::ShaderHelper::CreatePixelShader(&myPixelShader, "Shaders/ShadowShader"))
		{
			return false;
		}

		return true;
	}

	void CShadowRenderer::Render(std::shared_ptr<CCamera> aCamera, const std::vector<ModelCollectionExtended>& someModels, int anIndex)
	{
		aCamera;
		someModels;

		HRESULT result = { 0 };
		D3D11_MAPPED_SUBRESOURCE bufferData = { 0 };
		auto context = CEngine::GetInstance()->GetDXDeviceContext();

		Matrix4x4f cameraTransform(aCamera->GetTransform());
		float4 cameraPos(aCamera->GetPosition() + aCamera->GetRenderOffset(), 1);
		cameraTransform.SetRow(4, cameraPos);

		myFrameBufferData.myCameraTransform = cameraTransform;
		myFrameBufferData.myToCamera = Matrix4x4f::GetFastInverse(cameraTransform);
		myFrameBufferData.myToProjection = aCamera->GetProjection();
		myFrameBufferData.myCameraPosition = cameraPos;

		ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
		result = context->Map(myFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
		if (FAILED(result))
		{
			return;
		}
		memcpy(bufferData.pData, &myFrameBufferData, sizeof(FrameBufferData));
		context->Unmap(myFrameBuffer, 0);
		context->VSSetConstantBuffers(0, 1, &myFrameBuffer);
		context->PSSetConstantBuffers(0, 1, &myFrameBuffer);

		if (anIndex == 0)
		{
			ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
			result = context->Map(myShadowFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
			if (FAILED(result))
			{
				return;
			}
			memcpy(bufferData.pData, &myFrameBufferData, sizeof(FrameBufferData));
			context->Unmap(myShadowFrameBuffer, 0);
			context->VSSetConstantBuffers(5, 1, &myShadowFrameBuffer);
			context->PSSetConstantBuffers(5, 1, &myShadowFrameBuffer);
		}

		for (auto instance : someModels)
		{
			CModel*& model = instance.myCollection.model;
			CAnimator*& animator = instance.myCollection.animator;
			if (!model)
			{
				continue;
			}
			myObjectBufferData.myToWorld = instance.myTransform;
			myObjectBufferData.myUVScale[0] = 1;
			myObjectBufferData.myUVScale[1] = 1;
			myObjectBufferData.myScale = Vector4f(instance.myScale, 1.0f);
			myObjectBufferData.myHasBones = 0;
			if (model->HasBones() && animator)
			{
				myObjectBufferData.myHasBones = 1;
				myObjectBufferData.myNumBones = (unsigned)model->GetSkeleton().myJoints.size();
				memcpy(&myObjectBufferData.myBones[0], &(animator->GetJointTransforms()[0]), sizeof(Matrix4x4f) * myObjectBufferData.myNumBones);
			}

			ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
			result = context->Map(myObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
			if (FAILED(result))
			{
				return;
			}

			memcpy(bufferData.pData, &myObjectBufferData, sizeof(ObjectBufferData));
			context->Unmap(myObjectBuffer, 0);

			for (auto& mesh : model->GetMeshes())
			{
				context->IASetPrimitiveTopology(mesh.myPrimitiveTopology);
				context->IASetInputLayout(mesh.myInputLayout);
				context->IASetVertexBuffers(0, 1, &mesh.myVertexBuffer, &mesh.myStride, &mesh.myOffset);
				context->IASetIndexBuffer(mesh.myIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

				context->VSSetConstantBuffers(1, 1, &myObjectBuffer);
				context->VSSetShader(mesh.myVertexShader, nullptr, 0);
				context->GSSetShader(nullptr, nullptr, 0);
				context->PSSetShader(myPixelShader, nullptr, 0);

				context->PSSetShaderResources(9, 3, mesh.myTexture);
				context->DrawIndexed(mesh.myNumberOfIndices, 0, 0);
			}
		}
	}
}
