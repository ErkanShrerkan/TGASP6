#include "pch.h"
#include "RenderManager.h"
#pragma comment(lib, "d3d11.lib")
#include <d3d11.h>

#include <array>
#include "Engine.h"
#include "Scene.h"

#include "DirectX11Framework.h"
#include "ContentLoader.h"

#include "DX11.h"
#include "TextureFactory.h"
#include "TextureHelper.h"

#include "Input.h"
#include "Camera.h"

#include "CDebugDrawer.h"
#include "EnvironmentLight.h"

#include "VFXManager.h"
#include <algorithm>
#include <Game\Postmaster.h>

constexpr auto PI = 3.14159265f;

namespace SE
{
#define DELTA_TIME CEngine::GetInstance()->GetDeltaTime();
	bool CRenderManager::Init(CDirectX11Framework* aFramework)
	{
		myPostProcessingData.shallowWaterColor = float3(37.f / 255, 114.f / 255, 17.f / 255);
		myPostProcessingData.deepWaterColor = float3(24.f / 255, 47.f / 255, 8.f / 255);
		myPostProcessingData.waterBorderColor = float3(153.f / 255, 175.f / 255, 147.f / 255);
		myPostProcessingData.depthMult = 10.f;
		myPostProcessingData.alphaMult = 25.f;

		if (!myForwardRenderer.Init(aFramework))
		{
			/* Error Message */
			return false;
		}

		if (!myFullscreenRenderer.Init())
		{
			/* Error Message */
			return false;
		}

		if (!myDeferredRenderer.Init())
		{
			/* Error Message */
			return false;
		}

		if (!myTextRenderer.Init())
		{
			/* Error Message */
			return false;
		}

		if (!myShadowRenderer.Init())
		{
			return false;
		}

		if (!myVFXRenderer.Init())
		{
			return false;
		}

		CDebugDrawer::GetInstance().Init();

		ID3D11Resource* backBufferResource = nullptr;
		aFramework->GetBackBuffer()->GetResource(&backBufferResource);
		ID3D11Texture2D* backBufferTexture = reinterpret_cast<ID3D11Texture2D*>(backBufferResource);
		if (!backBufferTexture)
		{
			/* Error Message */
			return false;
		}

		myDeferredRenderer.SetVertexShader(myFullscreenRenderer.GetVertexShader());

		HRESULT result;
		D3D11_BUFFER_DESC bufferDesc = { 0 };
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		bufferDesc.ByteWidth = sizeof(PostProcessingData);
		result = CEngine::GetInstance()->GetDXDevice()->CreateBuffer(&bufferDesc, nullptr, &myPostProcessingBuffer);
		assert(SUCCEEDED(result));

		// settings
		ID3D11Device* device = CEngine::GetInstance()->GetDXDevice();

		D3D11_BLEND_DESC alphaBlendDesc = { };
		alphaBlendDesc.RenderTarget[0].BlendEnable = true;
		alphaBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		alphaBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		alphaBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		alphaBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		alphaBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		alphaBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
		alphaBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		result = device->CreateBlendState(&alphaBlendDesc, &myBlendStates[E_BLENDSTATE_ALPHABLEND]);
		assert(SUCCEEDED(result));

		D3D11_BLEND_DESC additiveBlendDesc = { };
		additiveBlendDesc.RenderTarget[0].BlendEnable = true;
		additiveBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		additiveBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		additiveBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		additiveBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		additiveBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		additiveBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
		additiveBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		result = device->CreateBlendState(&additiveBlendDesc, &myBlendStates[E_BLENDSTATE_ADDITIVE]);
		assert(SUCCEEDED(result));

		myBlendStates[E_BLENDSTATE_DISABLE] = nullptr;

		D3D11_DEPTH_STENCIL_DESC readOnlyDepthDesc = {};
		readOnlyDepthDesc.DepthEnable = true;
		readOnlyDepthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		readOnlyDepthDesc.DepthFunc = D3D11_COMPARISON_LESS;
		readOnlyDepthDesc.StencilEnable = false;

		result = device->CreateDepthStencilState(&readOnlyDepthDesc, &myDepthStencilStates[E_DEPTHSTENCILSTATE_READONLY]);
		assert(SUCCEEDED(result));

		myDepthStencilStates[E_DEPTHSTENCILSTATE_DEFAULT] = nullptr;

		D3D11_RASTERIZER_DESC wireframeRasterizerDesc = {};
		wireframeRasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
		wireframeRasterizerDesc.CullMode = D3D11_CULL_BACK;
		wireframeRasterizerDesc.DepthClipEnable = true;

		result = device->CreateRasterizerState(&wireframeRasterizerDesc, &myRasterizerStates[E_RASTERIZERSTATE_WIREFRAME]);
		assert(SUCCEEDED(result));

		D3D11_RASTERIZER_DESC reflectionDesc;
		reflectionDesc.FillMode = D3D11_FILL_SOLID;
		reflectionDesc.CullMode = D3D11_CULL_FRONT;
		reflectionDesc.DepthClipEnable = false;
		result = device->CreateRasterizerState(&reflectionDesc, &myRasterizerStates[E_RASTERIZERSTATE_CULLFRONT]);
		assert(SUCCEEDED(result));

		myRasterizerStates[E_RASTERIZERSTATE_DEFAULT] = nullptr;

		D3D11_SAMPLER_DESC pointSampleDesc = {};
		pointSampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		pointSampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		pointSampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		pointSampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		pointSampleDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		pointSampleDesc.MinLOD = -FLT_MAX;
		pointSampleDesc.MaxLOD = FLT_MAX;

		result = device->CreateSamplerState(&pointSampleDesc, &mySamplerStates[E_SAMPLERSTATE_POINT]);
		assert(SUCCEEDED(result));

		D3D11_SAMPLER_DESC trilWrapSampleDesc = {};
		trilWrapSampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		trilWrapSampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		trilWrapSampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		trilWrapSampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		trilWrapSampleDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		trilWrapSampleDesc.MinLOD = -FLT_MAX;
		trilWrapSampleDesc.MaxLOD = FLT_MAX;

		result = device->CreateSamplerState(&trilWrapSampleDesc, &mySamplerStates[E_SAMPLERSTATE_TRILINEARWRAP]);
		assert(SUCCEEDED(result));

		mySamplerStates[E_SAMPLERSTATE_TRILINEAR] = nullptr;

		SetSamplerState(ESamplerState::E_SAMPLERSTATE_TRILINEAR, 0);
		SetSamplerState(ESamplerState::E_SAMPLERSTATE_TRILINEARWRAP, 1);

		CreateTextures();
		CContentLoader* const& content = CEngine::GetInstance()->GetContentLoader();
		myBackBuffer = content->Load(backBufferTexture);
		Helper::TextureHelper::LoadShaderResourceView(&myWaterNormal, "models/EN_M_Water/EN_M_Water_N.dds");
		Helper::TextureHelper::LoadShaderResourceView(&myWaterAlbedo, "models/EN_M_Water/EN_M_Water_C.dds");
		Helper::TextureHelper::LoadShaderResourceView(&myWaterMaterial, "models/EN_M_Water/EN_M_Water_M.dds");
		Helper::TextureHelper::LoadShaderResourceView(&myWaterNormal2, "textures/water_n.dds");
		Postmaster::GetInstance()->Subscribe(this, eMessage::eUpdateResolution);
		return true;
	}

	void CRenderManager::RenderFrame()
	{
		//for (int i = int(eButtonInput::ToggleAllPasses); i < int(eButtonInput::ToggleAllPasses) + 8; i++)
		//{
		//	if (Input::GetInputPressed((eButtonInput)i))
		//	{
		//		myRenderLayer = i - int(eButtonInput::ToggleAllPasses);
		//		break;
		//	}
		//}

		if (Input::GetInputPressed(eButtonInput::ToggleMaterialPass))
		{
			myRenderLayer++;
			if (myRenderLayer > 7)
			{
				myRenderLayer = 0;
			}
		}

		myBackBuffer.ClearTexture({ 1, 1, 1, 1 });
		myIntermediateTexture.ClearTexture();
		myIntermediateDepth.ClearDepth();
		myBackBuffer.ClearTexture();
		myDeferredTexture.ClearTexture();
		myGBuffer.ClearTextures();
		myWaterBuffer.ClearTextures();
		myReflectionBuffer.ClearTextures();
		myReflectionTexture.ClearTexture();
		myFullscreenCopy.ClearTexture();
		myEffectTexture.ClearTexture();
		for (int i = 0; i < 2; i++)
		{
			myModelEffectTextures[i].ClearTexture();
		}

		D3D11_TEXTURE2D_DESC td;
		myDeferredTexture.GetTexture()->GetDesc(&td);
		Vector2ui res = DX11::GetResolution();
		myPostProcessingData.myTextureSize.x = td.Width;
		myPostProcessingData.myTextureSize.y = td.Height;
		myPostProcessingData.resolution.x = res.x;
		myPostProcessingData.resolution.y = res.y;

		//myPostProcessingData.shallowWaterColor = float3(37.f / 255, 114.f / 255, 17.f / 255);
		//myPostProcessingData.deepWaterColor = float3(24.f / 255, 47.f / 255, 8.f / 255);
		//myPostProcessingData.waterBorderColor = float3(153.f / 255, 175.f / 255, 147.f / 255);
		//myPostProcessingData.depthMult = 10.f;
		//myPostProcessingData.alphaMult = 25.f;
		//myPostProcessingData.borderThreshhold = 0.005f;

		myPostProcessingData.TimePI += DELTA_TIME;
		myPostProcessingData.deltaTime = DELTA_TIME;
		//if (myPostProcessingData.TimePI >= PI * 2)
		//{
		//	myPostProcessingData.TimePI -= PI * 2;
		//}
		//myPostProcessingData.waterLevel = myPostProcessingData.waterLevel + cos(myPostProcessingData.TimePI) * 5;

		D3D11_MAPPED_SUBRESOURCE bufferData = { 0 };
		ID3D11DeviceContext* context = CEngine::GetInstance()->GetDXDeviceContext();
		ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
		HRESULT result = context->Map(myPostProcessingBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
		if (FAILED(result))
		{
			return;
		}

		memcpy(bufferData.pData, &myPostProcessingData, sizeof(PostProcessingData));
		context->Unmap(myPostProcessingBuffer, 0);
		context->VSSetConstantBuffers(3, 1, &myPostProcessingBuffer);
		context->PSSetConstantBuffers(3, 1, &myPostProcessingBuffer);

		//PIXBeginEvent(PIX_COLOR_INDEX(2), "## Create Light vectors");
		// TODO: Fix flaskhals? :)
		// This is causing massive allocations & deallocations EVERY frame
		//std::vector<std::pair<size_t, std::array<CPointLight*, MAX_POINT_LIGHTS>>> pointLights;
		//std::vector<std::pair<size_t, std::array<CSpotLight*, MAX_SPOT_LIGHTS>>> spotLights;
		// A solution to this problem could be to let every model instance to hold
		// the arrays of lights itself and let the Cull method modify those arrays.
		// Don't know if this will increase FPS as the cache misses might do too
		// much damage if we keep the allocations down but spread out.
		// - Jesper 08/09/2021 
		//           ^  ^  ^ 
		//      rarted date format
		// 
		// Not a problem with DEFERRED RENDERING! :sunglasses:
		// - Eric 21/09/20
		//PIXEndEvent();

		CScene* const& scene = CEngine::GetInstance()->GetActiveScene();
		CCamera* const& mainCam = scene->GetMainCamera();

		// reflection cam setup
		Matrix4x4f orgCamMat = mainCam->GetTransform();
		float4 orgCamPos = orgCamMat.GetRow(4);
		float camHeight = mainCam->GetPosition().y + mainCam->GetRenderOffset().y;
		float waterHeight = myPostProcessingData.waterLevel;
		float dist = camHeight - waterHeight;
		float4 refCamPos = { orgCamPos.x, waterHeight - dist, orgCamPos.z, 1 };
		Matrix4x4f mirror;
		mirror(2, 2) = -1;
		Matrix4x4f mirroredCamMat = orgCamMat * mirror;
		mirroredCamMat.SetRow(2, mirroredCamMat.GetRow(2) * -1.f);
		mirroredCamMat.SetRow(4, refCamPos);
		//

		std::vector<CVFXManager::SVFXCollection> vfxs = CVFXManager::GetInstance().GetVFXs();
		const std::vector<ModelCollectionExtended>& modelsToRender = scene->GetModels();
		const std::vector<CPointLight*>& pointLights = scene->CullPointLights();
		const std::vector<CSpotLight*>& spotLights = scene->CullSpotLights();

		auto environmentLight = scene->GetEnvironmentLight();
		auto& shadowCam = environmentLight->GetShadowCam();
		shadowCam->SetRenderOffset(mainCam->GetRenderOffset());
		auto& camProjs = environmentLight->GetShadowCamProjections();
		auto& shadowTextures = environmentLight->GetShadowTextures();

		//if (Input::GetInputPressed(eButtonInput::AoeAttack))
		//{
		//	Postmaster::GetInstance()->SendMail(eMessage::e960x540);
		//}
		//if (Input::GetInputPressed(eButtonInput::Jump))
		//{
		//	Postmaster::GetInstance()->SendMail(eMessage::e1280x720);
		//}
		//if (Input::GetInputPressed(eButtonInput::Dammsugaren))
		//{
		//	Postmaster::GetInstance()->SendMail(eMessage::e1600x900);
		//}
		//if (Input::GetInputPressed(eButtonInput::UltimateAttack))
		//{
		//	Postmaster::GetInstance()->SendMail(eMessage::e1920x1080);
		//}
		//if (Input::GetInputPressed(eButtonInput::Heal))
		//{
		//	Postmaster::GetInstance()->SendMail(eMessage::e2560x1440);
		//}
		//if (Input::GetInputPressed(eButtonInput::RMBAttack))
		//{
		//	CTransform t;
		//	t.SetPosition(mainCam->GetRenderOffset() + float3(0, 25, 0));
		//	CVFXManager::GetInstance().PlayVFX("RMB", t);
		//}
		//if (Input::GetInputPressed(eButtonInput::SpawnEffect))
		//{
		//	CTransform t;
		//	t.SetPosition(mainCam->GetRenderOffset() + float3(0, 25, 0));
		//	CVFXManager::GetInstance().PlayVFX("Souls", t);
		//}

		for (int i = (int)shadowTextures.size() - 1; i > -1; i--)
		{
			shadowTextures[i].ClearTexture();
			shadowTextures[i].SetAsActiveTarget(1, &myIntermediateDepth);
			shadowCam->SetProjection(camProjs[i]);
			if (myRenderShadows)
			{
				myShadowRenderer.Render(shadowCam, modelsToRender, i);
			}
			myIntermediateDepth.ClearDepth();
		}

		// reflection buffer
		mainCam->SetTransform(mirroredCamMat);
		myReflectionBuffer.SetAsActiveTarget(&myIntermediateDepth);
		myDeferredRenderer.GenerateReflectionBuffer(mainCam, modelsToRender);
		mainCam->SetTransform(orgCamMat);
		myIntermediateDepth.ClearDepth();

		// pbr for reflections
		SetBlendState(E_BLENDSTATE_ADDITIVE);
		myReflectionTexture.SetAsActiveTarget();
		myReflectionBuffer.SetAllAsResources();

		myDeferredRenderer.Render(
			mainCam,
			environmentLight,
			pointLights,
			spotLights);

		SetBlendState(E_BLENDSTATE_DISABLE);
		myIntermediateDepth.ClearDepth();

		// Flip to culling
		//D3D11_CULL_MODE::D3D11_CULL_FRONT;
		// Orignal culling
		//D3D11_CULL_MODE::D3D11_CULL_BACK;

		//SetRasterizerState(ERasterizerState::E_RASTERIZERSTATE_WIREFRAME);
		// object buffer
		myGBuffer.SetAsActiveTarget(&myIntermediateDepth);
		myDeferredRenderer.GenerateGBuffer(mainCam, modelsToRender);
		SetRasterizerState(ERasterizerState::E_RASTERIZERSTATE_DEFAULT);

		// pbr for object buffer
		SetBlendState(E_BLENDSTATE_ADDITIVE);
		myDeferredTexture.SetAsActiveTarget();
		myGBuffer.SetAllAsResources();
		for (int i = 0; i < 2; i++)
		{
			shadowTextures[i].SetAsResourceOnSlot(20 + i);
		}

		myDeferredRenderer.Render(
			mainCam,
			environmentLight,
			pointLights,
			spotLights);
		SetBlendState(E_BLENDSTATE_DISABLE);

		// water layer from object pbr buffer
		myWaterBuffer.SetAsActiveTarget();
		myDeferredTexture.SetAsResourceOnSlot(0); // contains pbr objects
		myReflectionTexture.SetAsResourceOnSlot(1); // reflection
		myGBuffer.SetAllAsResources(2);
		SE::CEngine::GetInstance()->GetDXDeviceContext()->PSSetShaderResources(14, 1, &myWaterAlbedo); // albedo2
		SE::CEngine::GetInstance()->GetDXDeviceContext()->PSSetShaderResources(15, 1, &myWaterNormal); // normal2
		SE::CEngine::GetInstance()->GetDXDeviceContext()->PSSetShaderResources(17, 1, &myWaterMaterial); // material2
		SE::CEngine::GetInstance()->GetDXDeviceContext()->PSSetShaderResources(18, 1, &myWaterNormal2); // ao2
		myFullscreenRenderer.Render(CFullscreenRenderer::EShader_WaterUniversal);

		// pbr for water layer
		SetBlendState(E_BLENDSTATE_ADDITIVE);
		myIntermediateTexture.SetAsActiveTarget();
		myWaterBuffer.SetAllAsResources();
		myDeferredRenderer.Render(
			mainCam,
			environmentLight,
			pointLights,
			spotLights);
		SetBlendState(E_BLENDSTATE_DISABLE);

		// puts water layer on object pbr frame
		myMergedWaterTexture.SetAsActiveTarget();
		myDeferredTexture.SetAsResourceOnSlot(0); // contains pbr objects
		myIntermediateTexture.SetAsResourceOnSlot(1); // contains pbr water
		myGBuffer.SetAllAsResources(2); // contains object textures
		myReflectionTexture.SetAsResourceOnSlot(3); // reflection
		myReflectionBuffer.SetAsResourceOnSlot(CGBuffer::E_POSITION, 4); // reflection pos
		SE::CEngine::GetInstance()->GetDXDeviceContext()->PSSetShaderResources(5, 1, &myWaterNormal);
		myWaterBuffer.SetAllAsResources(13); // contains water textures
		myFullscreenRenderer.Render(CFullscreenRenderer::EShader_WaterMerge);

		myFullscreenCopy.SetAsActiveTarget();
		myDeferredTexture.SetAsActiveTarget(0);
		myFullscreenRenderer.Render(CFullscreenRenderer::EShader_Copy);

		// copies merged texture to deferred texture
		myDeferredTexture.SetAsActiveTarget();
		myMergedWaterTexture.SetAsResourceOnSlot(0);
		myFullscreenRenderer.Render(CFullscreenRenderer::EShader_Copy);

		// effects
		myEffectTexture.SetAsActiveTarget(1, &myIntermediateDepth);
		SetBlendState(E_BLENDSTATE_ADDITIVE);
		SetDepthStencilState(EDepthStencilState::E_DEPTHSTENCILSTATE_READONLY);
		myVFXRenderer.Render(mainCam, vfxs);
		SetDepthStencilState(EDepthStencilState::E_DEPTHSTENCILSTATE_DEFAULT);

		myFullscreenCopy.SetAsActiveTarget();
		myDeferredTexture.SetAsResourceOnSlot(0);
		myEffectTexture.SetAsResourceOnSlot(1);
		myFullscreenRenderer.Render(CFullscreenRenderer::EShader_AdditiveBlend);
		SetBlendState(E_BLENDSTATE_DISABLE);

		myDeferredTexture.SetAsActiveTarget();
		myFullscreenCopy.SetAsResourceOnSlot(0);
		myFullscreenRenderer.Render(CFullscreenRenderer::EShader_Copy);

		if (myRenderLayer == 0)
		{
			myLuminanceTexture.SetAsActiveTarget();
			myDeferredTexture.SetAsResourceOnSlot(0);
			myFullscreenRenderer.Render(CFullscreenRenderer::EShader_Luminance);

			myHalfSizeTexture.SetAsActiveTarget();
			myLuminanceTexture.SetAsResourceOnSlot(0);
			myFullscreenRenderer.Render(CFullscreenRenderer::EShader_Copy);

			myQuarterSizeTexture.SetAsActiveTarget();
			myHalfSizeTexture.SetAsResourceOnSlot(0);
			myFullscreenRenderer.Render(CFullscreenRenderer::EShader_Copy);

			myBlurTexture1.SetAsActiveTarget();
			myQuarterSizeTexture.SetAsResourceOnSlot(0);
			myFullscreenRenderer.Render(CFullscreenRenderer::EShader_Copy);

			for (size_t i = 0; i < 2; i++)
			{
				myBlurTexture2.SetAsActiveTarget();
				myBlurTexture1.SetAsResourceOnSlot(0);
				myFullscreenRenderer.Render(CFullscreenRenderer::EShader_GaussianHorizontal);

				myBlurTexture1.SetAsActiveTarget();
				myBlurTexture2.SetAsResourceOnSlot(0);
				myFullscreenRenderer.Render(CFullscreenRenderer::EShader_GaussianVertical);
			}

			myQuarterSizeTexture.SetAsActiveTarget();
			myBlurTexture1.SetAsResourceOnSlot(0);
			myFullscreenRenderer.Render(CFullscreenRenderer::EShader_Copy);

			myHalfSizeTexture.SetAsActiveTarget();
			myQuarterSizeTexture.SetAsResourceOnSlot(0);
			myFullscreenRenderer.Render(CFullscreenRenderer::EShader_Copy);

			myFullscreenCopy.SetAsActiveTarget();
			myDeferredTexture.SetAsResourceOnSlot(0);
			myHalfSizeTexture.SetAsResourceOnSlot(1);
			myFullscreenRenderer.Render(CFullscreenRenderer::EShader_Bloom);

			// particles
			myDeferredTexture.SetAsActiveTarget(1, &myIntermediateDepth);
			SetDepthStencilState(E_DEPTHSTENCILSTATE_READONLY);
			SetBlendState(E_BLENDSTATE_ADDITIVE);
			myParticleRenderer.Render(scene->GetParticleEmitters());
			SetBlendState(E_BLENDSTATE_DISABLE);
			SetDepthStencilState(E_DEPTHSTENCILSTATE_DEFAULT);

			// fog
			myFullscreenCopy.SetAsActiveTarget();
			myDeferredTexture.SetAsResourceOnSlot(0);
			myFullscreenRenderer.Render(CFullscreenRenderer::EShader_Copy);
			myGBuffer.SetAsResourceOnSlot(CGBuffer::E_POSITION, 2);
			myGBuffer.SetAsResourceOnSlot(CGBuffer::E_DEPTH, 8);
			myWaterBuffer.SetAsResourceOnSlot(CGBuffer::E_POSITION, 13);
			myWaterBuffer.SetAsResourceOnSlot(CGBuffer::E_DEPTH, 19);
			myFullscreenRenderer.Render(CFullscreenRenderer::EShader_Fog);

			SetBlendState(E_BLENDSTATE_DISABLE);
			myGBuffer.SetAllAsResources(2);
			int idx = 0;
			for (int i = 2; i < eShaderCount; i *= 2)
			{
				std::vector<ModelCollectionExtended> models;
				std::copy_if(modelsToRender.begin(), modelsToRender.end(), std::back_inserter(models),
					[&](const ModelCollectionExtended& mc) { return mc.myCollection.shaderType & i; });

				if (models.empty())
				{
					continue;
				}

				CFullscreenRenderer::EShader shader;
				switch (ShaderType(i))
				{
				case SE::eOutline:
					shader = CFullscreenRenderer::EShader_Outline;
					break;
				case SE::eHighlight:
					shader = CFullscreenRenderer::EShader_Highlight;
					break;
				default:
					continue;
					break;
				}

				myIntermediateDepth.ClearDepth();
				myModelEffectBuffer.ClearTextures();
				myModelEffectBuffer.SetAsActiveTarget(&myIntermediateDepth);
				myDeferredRenderer.GeneratePPBuffer(mainCam, models);

				myModelEffectTextures[idx].SetAsActiveTarget();
				myModelEffectBuffer.SetAllAsResources(13);
				myFullscreenRenderer.Render(shader);

				idx++;
			}

			//myFullscreenCopy.SetAsActiveTarget();
			//myDeferredTexture.SetAsResourceOnSlot(0);

			bool texture = true;
			CFullscreenTexture* textures[2] = { &myFullscreenCopy, &myDeferredTexture };

			for (int i = 0; i < 2; i++)
			{
				textures[texture]->SetAsActiveTarget();
				textures[!texture]->SetAsResourceOnSlot(0);
				myModelEffectTextures[i].SetAsResourceOnSlot(1);
				myFullscreenRenderer.Render(CFullscreenRenderer::EShader_AlphaBlend);
				texture = !texture;
			}

			myScaledBackBuffer.SetAsActiveTarget();
			CDebugDrawer::GetInstance().Render();
			//myModelEffectTextures[0].SetAsResourceOnSlot(0);
			//myFullscreenRenderer.Render(CFullscreenRenderer::EShader_Copy);
			myGBuffer.SetAllAsResources(2); // contains object textures
			//myFullscreenCopy.SetAsResourceOnSlot(0);
			textures[!texture]->SetAsResourceOnSlot(0);
			//myModelEffectTextures[1].SetAsResourceOnSlot(0);
			//myModelEffectBuffer.SetAsResourceOnSlot(CGBuffer::EGBufferTexture::E_DEPTH, 0);
			myFullscreenRenderer.Render(CFullscreenRenderer::EShader_DebugSpheres);

			SetBlendState(E_BLENDSTATE_ALPHABLEND);
			myForwardRenderer.RenderSprites(scene->GetSprites());
			myTextRenderer.Render(mainCam, scene->GetTexts());
			SetBlendState(E_BLENDSTATE_DISABLE);

			myBackBuffer.SetAsActiveTarget();
			myScaledBackBuffer.SetAsResourceOnSlot(0);
			myFullscreenRenderer.Render(CFullscreenRenderer::EShader_Copy);
		}
		else
		{
			myBackBuffer.SetAsActiveTarget();
			myGBuffer.SetAsResourceOnSlot((CGBuffer::EGBufferTexture)(myRenderLayer - 1), 0);
			//shadowTextures[myRenderLayer - 1].SetAsResourceOnSlot(0);
			myFullscreenRenderer.Render(CFullscreenRenderer::EShader_Copy);
		}
	}

	void CRenderManager::RecieveMessage(eMessage aMsg)
	{
		switch (aMsg)
		{
		case eMessage::eUpdateResolution:
			CreateTextures();
			break;
		default:
			break;
		}
	}

	void CRenderManager::CreateTextures()
	{
		Vector2ui res = DX11::GetResolution();
		CContentLoader* const& content = CEngine::GetInstance()->GetContentLoader();

		myGBuffer.Release();
		myWaterBuffer.Release();
		myModelEffectBuffer.Release();
		myReflectionBuffer.Release();
		myMergedWaterTexture.Release();
		myReflectionTexture.Release();
		myScaledBackBuffer.Release();
		myDeferredTexture.Release();
		myIntermediateDepth.Release();
		myIntermediateTexture.Release();
		myHalfSizeTexture.Release();
		myLuminanceTexture.Release();
		myQuarterSizeTexture.Release();
		myBlurTexture1.Release();
		myBlurTexture2.Release();
		myFullscreenCopy.Release();
		myEffectTexture.Release();
		myModelEffectTextures[0].Release();
		myModelEffectTextures[1].Release();

		myGBuffer = content->GetTextureFactory().CreateGBuffer(res);
		myWaterBuffer = content->GetTextureFactory().CreateGBuffer(res);
		myModelEffectBuffer = content->GetTextureFactory().CreateGBuffer(res);
		myReflectionBuffer = content->GetTextureFactory().CreateGBuffer(res);
		myMergedWaterTexture = content->Load(res, DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);
		myReflectionTexture = content->Load(res, DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);
		myScaledBackBuffer = content->Load(res, DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);
		myDeferredTexture = content->Load(res, DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);
		myIntermediateDepth = content->GetTextureFactory().CreateFullscreenDepth(res, DXGI_FORMAT_R32_TYPELESS);
		myIntermediateTexture = content->Load(res, DXGI_FORMAT_R8G8B8A8_UNORM);
		myHalfSizeTexture = content->Load({ res.x / 2, res.y / 2 }, DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);
		myLuminanceTexture = content->Load(res, DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);
		myQuarterSizeTexture = content->Load({ res.x / 4, res.y / 4 }, DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);
		myBlurTexture1 = content->Load(res, DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);
		myBlurTexture2 = content->Load(res, DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);
		myFullscreenCopy = content->Load(res, DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);
		myEffectTexture = content->Load(res, DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);
		myModelEffectTextures[0] = content->Load(res, DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);
		myModelEffectTextures[1] = content->Load(res, DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);
	}

	void CRenderManager::SetBlendState(EBlendState aBlendState)
	{
		CEngine::GetInstance()->GetDXDeviceContext()->OMSetBlendState(myBlendStates[aBlendState], 0, 0xffffffff);
	}

	void CRenderManager::SetDepthStencilState(EDepthStencilState aStencilState)
	{
		CEngine::GetInstance()->GetDXDeviceContext()->OMSetDepthStencilState(myDepthStencilStates[aStencilState], 1);
	}
	void CRenderManager::SetRasterizerState(ERasterizerState aRasterizerState)
	{
		CEngine::GetInstance()->GetDXDeviceContext()->RSSetState(myRasterizerStates[aRasterizerState]);
	}
	void CRenderManager::SetSamplerState(ESamplerState aSamplerState, int aSlot)
	{
		CEngine::GetInstance()->GetDXDeviceContext()->PSSetSamplers(aSlot, 1, &mySamplerStates[aSamplerState]);
	}
}
