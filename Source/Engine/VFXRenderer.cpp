#include "pch.h"
#include "VFXRenderer.h"
#include "DX11.h"
#include <d3d11.h>
#include "Engine\Engine.h"
#include "ShaderHelper.h"
#include "Camera.h"
#include "Engine\DebugProfiler.h"
#include "Model.h"
#include "VFXInstance.h"
#include "VFX.h"
#include <d3d11.h>

namespace SE
{
	CVFXRenderer::~CVFXRenderer()
	{
		myFrameBuffer->Release();
		myObjectBuffer->Release();
		myPixelShader->Release();
		myVertexShader->Release();
	}

	bool CVFXRenderer::Init()
	{
		ID3D11Device* device = CEngine::GetInstance()->GetDXDevice();

		HRESULT result;
		D3D11_BUFFER_DESC bufferDescription = { 0 };
		bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
		bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		bufferDescription.ByteWidth = sizeof(SFrameBufferData);
		result = device->CreateBuffer(&bufferDescription, nullptr, &myFrameBuffer);
		if (FAILED(result))
		{
			/* Error message */
			return false;
		}

		bufferDescription.ByteWidth = sizeof(SObjectBufferData);
		result = device->CreateBuffer(&bufferDescription, nullptr, &myObjectBuffer);
		if (FAILED(result))
		{
			/* Error message */
			return false;
		}

		Helper::ShaderHelper::CreatePixelShader(&myPixelShader, "Shaders/EffectPixelShader");
		Helper::ShaderHelper::CreateVertexShader(&myVertexShader, "Shaders/EffectVertexShader");
		myFrameBufferData.myCameraPosition.w = 1;
		return true;
	}

	void CVFXRenderer::Render(CCamera* aCamera, std::vector<CVFXManager::SVFXCollection>& someVFXs)
	{
		HRESULT result;
		D3D11_MAPPED_SUBRESOURCE bufferData;

		Matrix4x4f cameraTransform(aCamera->GetTransform());
		float4 cameraPos(aCamera->GetPosition() + aCamera->GetRenderOffset(), 1);
		cameraTransform.SetRow(4, cameraPos);

		myFrameBufferData.myCameraTransform = cameraTransform;
		myFrameBufferData.myToCamera = Matrix4x4f::GetFastInverse(cameraTransform);
		myFrameBufferData.myToProjection = aCamera->GetProjection();
		myFrameBufferData.myCameraPosition = cameraPos;

		auto& context = CEngine::GetInstance()->GetDXDeviceContext();

		ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
		result = context->Map(myFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
		if (FAILED(result))
		{
			/* Error Message */
			return;
		}
		memcpy(bufferData.pData, &myFrameBufferData, sizeof(SFrameBufferData));
		context->Unmap(myFrameBuffer, 0);
		context->VSSetConstantBuffers(0, 1, &myFrameBuffer);
		context->PSSetConstantBuffers(0, 1, &myFrameBuffer);

		for (auto& vfxc : someVFXs)
		{
			// set blendstate
			auto& vfx = vfxc.vfx->myVFX;
			for (int i = 0; i < vfx->myData.size(); i++)
			{
				auto& component = vfx->myData[i];
				CModel* model = component.model;
				if (!model || vfxc.vfx->myDeadComponents[i])
				{
					continue;
				}

				myObjectBufferData.myToWorld = vfxc.transform.GetTransform();
				myObjectBufferData.myColor = vfxc.vfx->myColors[i];
				myObjectBufferData.myScale = vfxc.vfx->myScales[i];
				myObjectBufferData.myUV = vfxc.vfx->myUVs[i];
				myObjectBufferData.myBlend = vfxc.vfx->myBlends[i];

				ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
				result = context->Map(myObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
				if (FAILED(result))
				{
					/* Error Message */
					return;
				}
				memcpy(bufferData.pData, &myObjectBufferData, sizeof(SObjectBufferData));
				context->Unmap(myObjectBuffer, 0);

				// Draw meshes
				for (auto& mesh : model->GetMeshes())
				{
					context->IASetPrimitiveTopology(mesh.myPrimitiveTopology);
					context->IASetInputLayout(mesh.myInputLayout);
					context->IASetVertexBuffers(0, 1, &mesh.myVertexBuffer, &mesh.myStride, &mesh.myOffset);
					context->IASetIndexBuffer(mesh.myIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

					context->VSSetConstantBuffers(1, 1, &myObjectBuffer);
					context->VSSetShader(myVertexShader, nullptr, 0);

					context->PSSetConstantBuffers(1, 1, &myObjectBuffer);
					auto srv = component.texture->GetShaderResourceView();
					context->PSSetShaderResources(0, 1, &srv);
					context->PSSetShader(myPixelShader, nullptr, 0);

					context->DrawIndexed(mesh.myNumberOfIndices, 0, 0);
					Singleton<Debug::CDebugProfiler>().IncrementDrawCallCount();
				}
			}
		}
	}
}