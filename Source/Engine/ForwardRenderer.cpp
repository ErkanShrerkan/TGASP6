#include "pch.h"
#include "ForwardRenderer.h"
#include "DirectX11Framework.h"
#include "ModelInstance.h"
#include "Model.h"
#include "Camera.h"
// Lights
#include "EnvironmentLight.h"
#include "PointLight.h"
#include "SpotLight.h"

enum ESlot {
	ESlot_SR_CubeMap,
	ESlot_SR_Textures,
};

#include "ShaderHelper.h"
#include "Sprite.h"
#include "Texture.h"

#include "DebugProfiler.h"

namespace SE
{
	CForwardRenderer::CForwardRenderer()
		: myContext(nullptr)
	{}

	CForwardRenderer::~CForwardRenderer()
	{
	}

	bool CForwardRenderer::Init(CDirectX11Framework* aFramework)
	{
		if (!aFramework)
		{
			/* Error Message */
			return false;
		}

		myContext = aFramework->GetContext();
		if (!myContext)
		{
			/* Error Message */
			return false;
		}

		ID3D11Device* device = aFramework->GetDevice();

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

		bufferDescription.ByteWidth = sizeof(SSpriteBufferData);
		result = device->CreateBuffer(&bufferDescription, nullptr, &mySpriteBuffer);
		if (FAILED(result))
		{
			/* Error message */
			return false;
		}

		Helper::ShaderHelper::CreatePixelShader(&mySpritePixelShader, "Shaders/SpritePixelShader");
		Helper::ShaderHelper::CreateVertexShader(&mySpriteVertexShader, "Shaders/SpriteVertexShader");
		Helper::ShaderHelper::CreatePixelShader(&myFisheyePixelShader, "Shaders/SpritePS-Fisheye");
		Helper::ShaderHelper::CreatePixelShader(&myCircularFillPixelShader, "Shaders/SpritePS-CircularFill");

		return true;
	}

	void CForwardRenderer::Render(
		const CEnvironmentLight* const anEnvironmentLight,
		const std::vector<std::pair<size_t, std::array<CPointLight*, MAX_POINT_LIGHTS>>>& somePointLights,
		const std::vector<std::pair<size_t, std::array<CSpotLight*, MAX_SPOT_LIGHTS>>>& someSpotLights,
		const CCamera* aCamera,
		std::vector<CModelInstance*>& aModelList)
	{
		HRESULT result;
		D3D11_MAPPED_SUBRESOURCE bufferData;

		ID3D11Resource* cubeResource;
		anEnvironmentLight->GetCubeMap()->GetResource(&cubeResource);
		ID3D11Texture2D* cubeTexture = reinterpret_cast<ID3D11Texture2D*>(cubeResource);
		D3D11_TEXTURE2D_DESC description;
		cubeTexture->GetDesc(&description);

		myFrameBufferData.myToCameraSpace = Matrix4x4f::GetFastInverse(aCamera->GetTransform());
		myFrameBufferData.myToProjectionSpace = aCamera->GetProjection();
		myFrameBufferData.myCameraPosition = aCamera->GetPosition();
		myFrameBufferData.myDirectionalLightDirection = anEnvironmentLight->GetDirection();
		myFrameBufferData.myDirectionalLightColor = anEnvironmentLight->GetDirectionalColor();
		myFrameBufferData.myEnvironmentLightMipCount = anEnvironmentLight->GetMipLevels();

		ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
		result = myContext->Map(myFrameBuffer.Raw(), 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
		if (FAILED(result))
		{
			/* Error Message */
			return;
		}
		memcpy(bufferData.pData, &myFrameBufferData, sizeof(SFrameBufferData));
		myContext->Unmap(myFrameBuffer.Raw(), 0);
		myContext->VSSetConstantBuffers(0, 1, &myFrameBuffer);
		myContext->PSSetConstantBuffers(0, 1, &myFrameBuffer);
		myContext->PSSetShaderResources(ESlot_SR_CubeMap, 1, &anEnvironmentLight->GetCubeMap());

		size_t index = 0;
		for (CModelInstance* instance : aModelList)
		{
			CModel* model = instance->GetModel();
			const std::pair<size_t, std::array<CPointLight*, MAX_POINT_LIGHTS>>& pointLights = somePointLights[index];
			const std::pair<size_t, std::array<CSpotLight*, MAX_SPOT_LIGHTS>>& spotLights = someSpotLights[index];
			uint amountOfPointLights = static_cast<uint>(pointLights.first);
			uint amountOfSpotLights = static_cast<uint>(spotLights.first);

			// Set ObjectBufferData
			myObjectBufferData.myToWorldSpace = instance->GetTransform();
			myObjectBufferData.myScale = Vector4f(instance->GetScale(), 1.0f);
			myObjectBufferData.myAmountOfPointsLights = amountOfPointLights;
			for (uint i = 0; i < amountOfPointLights; ++i)
			{
				myObjectBufferData.myPointLights[i].myColor = pointLights.second[i]->GetBundledColor();
				myObjectBufferData.myPointLights[i].myPosition = pointLights.second[i]->GetBundledPosition();
			}
			myObjectBufferData.myAmountOfSpotLights = amountOfSpotLights;
			for (uint i = 0; i < amountOfSpotLights; ++i)
			{
				myObjectBufferData.mySpotLights[i].myColor = spotLights.second[i]->GetBundledColor();
				myObjectBufferData.mySpotLights[i].myPosition = spotLights.second[i]->GetBundledPosition();
				myObjectBufferData.mySpotLights[i].myDirection = spotLights.second[i]->GetRawBundledDirection();
				myObjectBufferData.mySpotLights[i].myInnerAngle = spotLights.second[i]->GetInnerAngle();
				myObjectBufferData.mySpotLights[i].myOuterAngle = spotLights.second[i]->GetOuterAngle();
			}

			// Put ObjectBufferData into DirectX
			ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
			result = myContext->Map(myObjectBuffer.Raw(), 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
			if (FAILED(result))
			{
				/* Error Message */
				return;
			}
			memcpy(bufferData.pData, &myObjectBufferData, sizeof(SObjectBufferData));
			myContext->Unmap(myObjectBuffer.Raw(), 0);

			// Draw meshes
			for (auto& mesh : model->GetMeshes())
			{
				myContext->IASetPrimitiveTopology(mesh.myPrimitiveTopology);
				myContext->IASetInputLayout(mesh.myInputLayout);
				myContext->IASetVertexBuffers(0, 1, &mesh.myVertexBuffer, &mesh.myStride, &mesh.myOffset);
				myContext->IASetIndexBuffer(mesh.myIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

				myContext->VSSetConstantBuffers(1, 1, &myObjectBuffer);
				myContext->VSSetShader(mesh.myVertexShader, nullptr, 0);

				myContext->PSSetConstantBuffers(1, 1, &myObjectBuffer);
				myContext->PSSetShaderResources(ESlot_SR_Textures, CModel::SMesh::ETextureMap_Count, mesh.myTexture);
				myContext->PSSetShader(mesh.myPixelShader, nullptr, 0);

				myContext->DrawIndexed(mesh.myNumberOfIndices, 0, 0);
				Singleton<Debug::CDebugProfiler>().IncrementDrawCallCount();
			}

			++index;
		}
	}

	void CForwardRenderer::RenderSprites(std::vector<CSprite*>& someSprites)
	{
		HRESULT result;

		myContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		myContext->VSSetShader(mySpriteVertexShader, nullptr, 0u);

		D3D11_MAPPED_SUBRESOURCE bufferData;
		for (CSprite*& sprite : someSprites)
		{
			mySpriteBufferData.myPosition = sprite->GetPosition();
			mySpriteBufferData.mySize = sprite->GetSize();
			mySpriteBufferData.myPivot = sprite->GetPivot();
			mySpriteBufferData.myRotation = sprite->GetRotation();
			mySpriteBufferData.myRect = sprite->GetRect();
			mySpriteBufferData.myData = sprite->GetShaderData();
			mySpriteBufferData.myColor = sprite->GetColor();
			mySpriteBufferData.myIsGamma = sprite->GetIsGamma();

			ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
			result = myContext->Map(mySpriteBuffer.Raw(), 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
			if (FAILED(result))
			{
				/* Error Message */
				return;
			}
			memcpy(bufferData.pData, &mySpriteBufferData, sizeof(SSpriteBufferData));
			myContext->Unmap(mySpriteBuffer.Raw(), 0);

			myContext->IASetInputLayout(sprite->myInputLayout);
			myContext->IASetIndexBuffer(sprite->myIndexBuffer, DXGI_FORMAT_R32_UINT, 0u);
			myContext->IASetVertexBuffers(0u, 1u, &sprite->myVertexBuffer, &sprite->myStride, &sprite->myOffset);
			myContext->PSSetShaderResources(0u, 1u, sprite->myTexture->GetPointerToShaderResourceView());
			myContext->PSSetShaderResources(1u, 1u, sprite->myMaskTexture->GetPointerToShaderResourceView());
			myContext->PSSetConstantBuffers(0u, 1u, &mySpriteBuffer);
			myContext->VSSetConstantBuffers(0u, 1u, &mySpriteBuffer);

			ID3D11PixelShader* pixelShader;

			switch (sprite->GetShaderType())
			{
			case SE::SpriteShaderType::eNone:
				pixelShader = mySpritePixelShader;
				break;
			case SE::SpriteShaderType::eCircularFill:
				pixelShader = myCircularFillPixelShader;
				break;
			case SE::SpriteShaderType::eFisheye:
				pixelShader = myFisheyePixelShader;
				break;
			default:
				pixelShader = mySpritePixelShader;
				break;
			}

			myContext->PSSetShader(pixelShader, nullptr, 0u);

			myContext->DrawIndexed(6u, 0u, 0u);
			Singleton<Debug::CDebugProfiler>().IncrementDrawCallCount();
		}
	}
}
