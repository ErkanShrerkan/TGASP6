#pragma once
#include <d3d11.h>
#include "Scene.h"

namespace SE
{
	class CCamera;
	class CEnvironmentLight;
	class CModelInstance;
	class CSpotLight;
	class CPointLight;

	class CDeferredRenderer
	{
	public:
		CDeferredRenderer();
		~CDeferredRenderer();

		bool Init();
		bool SetCameraAndEnvironment(
			CCamera* aCamera,
			CEnvironmentLight* anEnvironmentLight);

		void GenerateGBuffer(
			CCamera* aCamera,
			std::vector<CModelInstance*> someModelInstances);

		void GenerateReflectionBuffer(
			CCamera* aCamera,
			std::vector<CModelInstance*> someModelInstances);

		void GenerateGBuffer(
			CCamera* aCamera,
			const std::vector<ModelCollectionExtended>& someModelInstances);

		void GeneratePPBuffer(
			CCamera* aCamera,
			const std::vector<ModelCollectionExtended>& someModelInstances);

		void GenerateReflectionBuffer(
			CCamera* aCamera,
			const std::vector<ModelCollectionExtended>& someModelInstances);

		void Render(
			CCamera* aCamera,
			CEnvironmentLight* anEnvironmentLight,
			const std::vector<CPointLight*>& somePointLights,
			const std::vector<CSpotLight*>& someSpotLights);

		void SetVertexShader(ID3D11VertexShader* aVS) { myFullscreenShader = aVS; }
		void SetPixelShader(ID3D11PixelShader* aPS = nullptr);

	private:

		struct FrameBufferData
		{
			Matrix4x4f myCameraTransform;
			Matrix4x4f myToCamera;
			Matrix4x4f myToProjection;
			Vector4f myCameraPosition;
		} myFrameBufferData;

		struct ObjectBufferData
		{
			Matrix4x4f myToWorld;
			float4 myScale;
			unsigned int myUVScale[2];
			unsigned int myHasBones;
			unsigned int myNumBones;
			Matrix4x4f myBones[128];
			float4 myColor;
		} myObjectBufferData;

		struct LightBufferData
		{
			Vector4f myPosition;
			Vector4f myColor;
			Vector4f myDirection;
			float myRange;
			float myInnerAngle;
			float myOuterAngle;
			float mySign;
		}myLightBufferData;

		ID3D11Buffer* myFrameBuffer = nullptr;
		ID3D11Buffer* myObjectBuffer = nullptr;
		ID3D11Buffer* myLightBuffer = nullptr;
		ID3D11PixelShader* myPixelShader = nullptr;
		ID3D11PixelShader* myPPPShader = nullptr;
		ID3D11PixelShader* myReflectionShader = nullptr;
		ID3D11PixelShader* myCurrentPixelShader = nullptr;
		ID3D11PixelShader* myLightShader = nullptr;
		ID3D11VertexShader* myFullscreenShader = nullptr;
	};
}

