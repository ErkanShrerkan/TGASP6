#include "pch.h"
#include "DeferredRenderer.h"
#include "Camera.h"
#include "ModelInstance.h"
#include "Model.h"
#include "EnvironmentLight.h"
#include "SpotLight.h"
#include "PointLight.h"
#include "Engine.h"
#include "Animator.h"
#include <fstream>
#include <cassert>
#include "ShaderHelper.h"

#include "DebugProfiler.h"

namespace SE
{
	CDeferredRenderer::CDeferredRenderer()
	{
	}

	CDeferredRenderer::~CDeferredRenderer()
	{
	}

	bool CDeferredRenderer::Init()
	{
		HRESULT result;

		D3D11_BUFFER_DESC bufferDesc = { 0 };
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		ID3D11Device* device = CEngine::GetInstance()->GetDXDevice();

		bufferDesc.ByteWidth = sizeof(FrameBufferData);
		result = device->CreateBuffer(&bufferDesc, nullptr, &myFrameBuffer);
		if (FAILED(result))
		{
			return false;
		}

		if (FAILED(result))
		{
			return false;
		}

		bufferDesc.ByteWidth = sizeof(ObjectBufferData);
		result = device->CreateBuffer(&bufferDesc, nullptr, &myObjectBuffer);

		if (FAILED(result))
		{
			return false;
		}

		bufferDesc.ByteWidth = sizeof(LightBufferData);
		result = device->CreateBuffer(&bufferDesc, nullptr, &myLightBuffer);
		if (FAILED(result))
		{
			return false;
		}

		if (!Helper::ShaderHelper::CreatePixelShader(&myPixelShader, "Shaders/GBuffer"))
		{
			return false;
		}
		myCurrentPixelShader = myPixelShader;

		if (!Helper::ShaderHelper::CreatePixelShader(&myPPPShader, "Shaders/PPBuffer"))
		{
			return false;
		}

		if (!Helper::ShaderHelper::CreatePixelShader(&myLightShader, "Shaders/LightShader"))
		{
			return false;
		}

		if (!Helper::ShaderHelper::CreatePixelShader(&myReflectionShader, "Shaders/ReflectionBuffer"))
		{
			return false;
		}

		return true;
	}

	bool CDeferredRenderer::SetCameraAndEnvironment(CCamera* aCamera, CEnvironmentLight* anEnvironmentLight)
	{
		aCamera;
		anEnvironmentLight;
		return true;
	}

	void CDeferredRenderer::GenerateGBuffer(CCamera* aCamera, std::vector<CModelInstance*> someModelInstances)
	{
		HRESULT result = { 0 };
		D3D11_MAPPED_SUBRESOURCE bufferData = { 0 };

		Matrix4x4f cameraTransform(aCamera->GetTransform());
		float4 cameraPos(aCamera->GetPosition() + aCamera->GetRenderOffset(), 1);
		cameraTransform.SetRow(4, cameraPos);

		myFrameBufferData.myCameraTransform = cameraTransform;
		myFrameBufferData.myToCamera = Matrix4x4f::GetFastInverse(cameraTransform);
		myFrameBufferData.myToProjection = aCamera->GetProjection();
		myFrameBufferData.myCameraPosition = cameraPos;

		ID3D11DeviceContext* context = CEngine::GetInstance()->GetDXDeviceContext();

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
		context->GSSetConstantBuffers(0, 1, &myFrameBuffer);

		for (auto instance : someModelInstances)
		{
			CModel* model = instance->GetModel();
			myObjectBufferData.myToWorld = instance->GetTransform();
			myObjectBufferData.myUVScale[0] = 1;
			myObjectBufferData.myUVScale[1] = 1;
			myObjectBufferData.myScale = Vector4f(instance->GetScale(), 1.0f);

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

				context->PSSetConstantBuffers(1, 1, &myObjectBuffer);
				context->PSSetShaderResources(9, 3, mesh.myTexture);

				context->PSSetShader(myCurrentPixelShader, nullptr, 0);

				context->DrawIndexed(mesh.myNumberOfIndices, 0, 0);
				Singleton<Debug::CDebugProfiler>().IncrementDrawCallCount();
			}
		}
	}

	void CDeferredRenderer::GenerateReflectionBuffer(CCamera* aCamera, std::vector<CModelInstance*> someModelInstances)
	{
		HRESULT result = { 0 };
		D3D11_MAPPED_SUBRESOURCE bufferData = { 0 };

		Matrix4x4f cameraTransform(aCamera->GetTransform());
		float4 cameraPos(aCamera->GetPosition() + aCamera->GetRenderOffset(), 1);
		cameraPos.y = aCamera->GetPosition().y;
		cameraTransform.SetRow(4, cameraPos);

		myFrameBufferData.myCameraTransform = cameraTransform;
		myFrameBufferData.myToCamera = Matrix4x4f::GetFastInverse(cameraTransform);
		myFrameBufferData.myToProjection = aCamera->GetProjection();
		myFrameBufferData.myCameraPosition = cameraPos;

		ID3D11DeviceContext* context = CEngine::GetInstance()->GetDXDeviceContext();

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

		for (auto instance : someModelInstances)
		{
			CModel* model = instance->GetModel();
			myObjectBufferData.myToWorld = instance->GetTransform();
			myObjectBufferData.myUVScale[0] = 1;
			myObjectBufferData.myUVScale[1] = 1;
			myObjectBufferData.myScale = Vector4f(instance->GetScale(), 1.0f);

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

				context->PSSetConstantBuffers(1, 1, &myObjectBuffer);
				context->PSSetShaderResources(9, 3, mesh.myTexture);

				context->PSSetShader(myReflectionShader, nullptr, 0);

				context->DrawIndexed(mesh.myNumberOfIndices, 0, 0);
				Singleton<Debug::CDebugProfiler>().IncrementDrawCallCount();
			}
		}
	}

	void CDeferredRenderer::GenerateGBuffer(CCamera* aCamera, const std::vector<ModelCollectionExtended>& someModelInstances)
	{
		HRESULT result = { 0 };
		D3D11_MAPPED_SUBRESOURCE bufferData = { 0 };

		Matrix4x4f cameraTransform(aCamera->GetTransform());
		float4 cameraPos(aCamera->GetPosition() + aCamera->GetRenderOffset(), 1);
		cameraTransform.SetRow(4, cameraPos);

		myFrameBufferData.myCameraTransform = cameraTransform;
		myFrameBufferData.myToCamera = Matrix4x4f::GetFastInverse(cameraTransform);
		myFrameBufferData.myToProjection = aCamera->GetProjection();
		myFrameBufferData.myCameraPosition = cameraPos;

		ID3D11DeviceContext* context = CEngine::GetInstance()->GetDXDeviceContext();

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

		for (auto instance : someModelInstances)
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
			myObjectBufferData.myColor = instance.myCollection.color;
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

			if (!model)
			{
				continue;
			}
			for (auto& mesh : model->GetMeshes())
			{
				context->IASetPrimitiveTopology(mesh.myPrimitiveTopology);
				context->IASetInputLayout(mesh.myInputLayout);
				context->IASetVertexBuffers(0, 1, &mesh.myVertexBuffer, &mesh.myStride, &mesh.myOffset);
				context->IASetIndexBuffer(mesh.myIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

				context->VSSetConstantBuffers(1, 1, &myObjectBuffer);
				context->VSSetShader(mesh.myVertexShader, nullptr, 0);

				context->PSSetConstantBuffers(1, 1, &myObjectBuffer);
				context->PSSetShaderResources(9, 3, mesh.myTexture);

				context->PSSetShader(myCurrentPixelShader, nullptr, 0);

				context->DrawIndexed(mesh.myNumberOfIndices, 0, 0);
				Singleton<Debug::CDebugProfiler>().IncrementDrawCallCount();
			}
		}
	}

	void CDeferredRenderer::GeneratePPBuffer(CCamera* aCamera, const std::vector<ModelCollectionExtended>& someModelInstances)
	{
		HRESULT result = { 0 };
		D3D11_MAPPED_SUBRESOURCE bufferData = { 0 };

		Matrix4x4f cameraTransform(aCamera->GetTransform());
		float4 cameraPos(aCamera->GetPosition() + aCamera->GetRenderOffset(), 1);
		cameraTransform.SetRow(4, cameraPos);

		myFrameBufferData.myCameraTransform = cameraTransform;
		myFrameBufferData.myToCamera = Matrix4x4f::GetFastInverse(cameraTransform);
		myFrameBufferData.myToProjection = aCamera->GetProjection();
		myFrameBufferData.myCameraPosition = cameraPos;

		ID3D11DeviceContext* context = CEngine::GetInstance()->GetDXDeviceContext();

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

		for (auto instance : someModelInstances)
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
			myObjectBufferData.myColor = instance.myCollection.color;
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

			if (!model)
			{
				continue;
			}
			for (auto& mesh : model->GetMeshes())
			{
				context->IASetPrimitiveTopology(mesh.myPrimitiveTopology);
				context->IASetInputLayout(mesh.myInputLayout);
				context->IASetVertexBuffers(0, 1, &mesh.myVertexBuffer, &mesh.myStride, &mesh.myOffset);
				context->IASetIndexBuffer(mesh.myIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

				context->VSSetConstantBuffers(1, 1, &myObjectBuffer);
				context->VSSetShader(mesh.myVertexShader, nullptr, 0);

				context->PSSetConstantBuffers(1, 1, &myObjectBuffer);
				context->PSSetShaderResources(9, 3, mesh.myTexture);

				context->PSSetShader(myPPPShader, nullptr, 0);

				context->DrawIndexed(mesh.myNumberOfIndices, 0, 0);
				Singleton<Debug::CDebugProfiler>().IncrementDrawCallCount();
			}
		}
	}

	void CDeferredRenderer::GenerateReflectionBuffer(CCamera* aCamera, const std::vector<ModelCollectionExtended>& someModelInstances)
	{
		HRESULT result = { 0 };
		D3D11_MAPPED_SUBRESOURCE bufferData = { 0 };

		Matrix4x4f cameraTransform(aCamera->GetTransform());
		float4 cameraPos(aCamera->GetPosition() + aCamera->GetRenderOffset(), 1);
		cameraPos.y = aCamera->GetPosition().y;
		cameraTransform.SetRow(4, cameraPos);

		myFrameBufferData.myCameraTransform = cameraTransform;
		myFrameBufferData.myToCamera = Matrix4x4f::GetFastInverse(cameraTransform);
		myFrameBufferData.myToProjection = aCamera->GetProjection();
		myFrameBufferData.myCameraPosition = cameraPos;

		ID3D11DeviceContext* context = CEngine::GetInstance()->GetDXDeviceContext();

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

		for (auto instance : someModelInstances)
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
			myObjectBufferData.myColor = instance.myCollection.color;
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

			if (!model)
			{
				continue;
			}
			for (auto& mesh : model->GetMeshes())
			{
				context->IASetPrimitiveTopology(mesh.myPrimitiveTopology);
				context->IASetInputLayout(mesh.myInputLayout);
				context->IASetVertexBuffers(0, 1, &mesh.myVertexBuffer, &mesh.myStride, &mesh.myOffset);
				context->IASetIndexBuffer(mesh.myIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

				context->VSSetConstantBuffers(1, 1, &myObjectBuffer);
				context->VSSetShader(mesh.myVertexShader, nullptr, 0);

				context->PSSetConstantBuffers(1, 1, &myObjectBuffer);
				context->PSSetShaderResources(9, 3, mesh.myTexture);

				context->PSSetShader(myReflectionShader, nullptr, 0);

				context->DrawIndexed(mesh.myNumberOfIndices, 0, 0);
				Singleton<Debug::CDebugProfiler>().IncrementDrawCallCount();
			}
		}
	}

	void CDeferredRenderer::Render(CCamera* aCamera, CEnvironmentLight* anEnvironmentLight, const std::vector<CPointLight*>& somePointLights, const std::vector<CSpotLight*>& someSpotLights)
	{
		aCamera;
		HRESULT result;
		D3D11_MAPPED_SUBRESOURCE bufferData = { 0 };

		myLightBufferData.myDirection = anEnvironmentLight->GetDirection() * -1.f;
		myLightBufferData.myColor = anEnvironmentLight->GetDirectionalColor();
		myLightBufferData.myPosition = anEnvironmentLight->GetAmbientColor();
		myLightBufferData.mySign = 0;

		ID3D11DeviceContext* context = CEngine::GetInstance()->GetDXDeviceContext();

		ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
		result = context->Map(myLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
		assert(SUCCEEDED(result));

		memcpy(bufferData.pData, &myLightBufferData, sizeof(LightBufferData));
		context->Unmap(myLightBuffer, 0);

		context->PSSetConstantBuffers(2, 1, &myLightBuffer);
		ID3D11ShaderResourceView* cmap = anEnvironmentLight->GetCubeMap();
		context->PSSetShaderResources(12, 1, &cmap);

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->IASetInputLayout(nullptr);
		context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
		context->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);

		context->VSSetShader(myFullscreenShader, nullptr, 0);

		context->PSSetShader(myLightShader, nullptr, 0);
		context->Draw(3, 0);
		Singleton<Debug::CDebugProfiler>().IncrementDrawCallCount();

		for (auto& light : somePointLights)
		{
			myLightBufferData.myColor = light->GetBundledColor();
			myLightBufferData.myPosition = light->GetBundledPosition();
			myLightBufferData.myRange = myLightBufferData.myPosition.w;
			myLightBufferData.myPosition.w = 1;
			myLightBufferData.mySign = 1;

			ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
			result = context->Map(myLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
			assert(SUCCEEDED(result));

			memcpy(bufferData.pData, &myLightBufferData, sizeof(LightBufferData));
			context->Unmap(myLightBuffer, 0);
			context->PSSetConstantBuffers(2, 1, &myLightBuffer);
			context->PSSetShader(myLightShader, nullptr, 0);
			context->Draw(3, 0);
			Singleton<Debug::CDebugProfiler>().IncrementDrawCallCount();
		}
		for (auto& light : someSpotLights)
		{
			myLightBufferData.myColor = light->GetBundledColor();
			myLightBufferData.myPosition = light->GetBundledPosition();
			myLightBufferData.myRange = myLightBufferData.myPosition.w;
			myLightBufferData.myPosition.w = 1;
			myLightBufferData.myDirection = light->GetBundledDirection();
			myLightBufferData.myInnerAngle = light->GetInnerAngle();
			myLightBufferData.myOuterAngle = light->GetOuterAngle();
			myLightBufferData.myDirection.w = 0;
			myLightBufferData.mySign = 2;

			ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
			result = context->Map(myLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
			assert(SUCCEEDED(result));

			memcpy(bufferData.pData, &myLightBufferData, sizeof(LightBufferData));
			context->Unmap(myLightBuffer, 0);
			context->PSSetConstantBuffers(2, 1, &myLightBuffer);
			context->PSSetShader(myLightShader, nullptr, 0);
			context->Draw(3, 0);
			Singleton<Debug::CDebugProfiler>().IncrementDrawCallCount();
		}

		myLightBufferData.myDirection = anEnvironmentLight->GetDirection() * -1.f;
		myLightBufferData.myColor = anEnvironmentLight->GetDirectionalColor();
		myLightBufferData.myPosition = anEnvironmentLight->GetAmbientColor();
		myLightBufferData.mySign = 0;

		ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
		result = context->Map(myLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
		assert(SUCCEEDED(result));

		memcpy(bufferData.pData, &myLightBufferData, sizeof(LightBufferData));
		context->Unmap(myLightBuffer, 0);

		context->PSSetConstantBuffers(2, 1, &myLightBuffer);
	}

	void CDeferredRenderer::SetPixelShader(ID3D11PixelShader* aPS)
	{
		if (aPS)
		{
			myCurrentPixelShader = aPS;
		}
		else
		{
			myCurrentPixelShader = myPixelShader;
		}
	}
}
