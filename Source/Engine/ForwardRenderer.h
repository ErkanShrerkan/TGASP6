#pragma once
#include "ShaderDefines.h"
#include "AutoReleaser.h"
#include <array>
struct ID3D11DeviceContext;
struct ID3D11Buffer;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
namespace SE
{
	class CDirectX11Framework;
	class CModelInstance;
	class CCamera;
	class CEnvironmentLight;
	class CPointLight;
	class CSpotLight;
	class CSprite;
	class CForwardRenderer
	{
	public:
		CForwardRenderer();
		~CForwardRenderer();

		bool Init(CDirectX11Framework* aFramework);
		void Render(
			const CEnvironmentLight *const anEnvironmentLight,
			const std::vector<std::pair<size_t, std::array<CPointLight*, MAX_POINT_LIGHTS>>>& somePointLights,
			const std::vector<std::pair<size_t, std::array<CSpotLight*, MAX_SPOT_LIGHTS>>>& someSpotLights,
			const CCamera* aCamera,
			std::vector<CModelInstance*>& aModelList);

		void RenderSprites(std::vector<CSprite*>& someSprites);

		//void SetCameraAndLight();
		//void Render(CModelInstance*,)

	private:
		struct SFrameBufferData
		{
			float4x4 myToCameraSpace;
			float4x4 myToProjectionSpace;
			float4 myDirectionalLightDirection;
			float4 myDirectionalLightColor;
			float3 myCameraPosition;
			float1 myEnvironmentLightMipCount;
		};
		struct SObjectBufferData
		{
			float4x4 myToWorldSpace;
			float4 myScale;

			uint myAmountOfPointsLights;
			uint myAmountOfSpotLights;
			uint2 garbachoUno;

			struct SPointLightData
			{
				float4 myPosition;
				float4 myColor;
			}
			myPointLights[MAX_POINT_LIGHTS];

			struct SSpotLightData
			{
				float4 myPosition;
				float4 myDirection;
				float4 myColor;
				float myInnerAngle;
				float myOuterAngle;
				float2 garbachoDos;
			}
			mySpotLights[MAX_SPOT_LIGHTS];
		};
		struct SSpriteBufferData
		{
			float4 myColor;
			float4 myRect;
			float2 myPosition;
			float2 mySize;
			float2 myPivot;
			float myRotation;
			float myData;
			int myIsGamma;
			int3 myTrash;
		};

		SFrameBufferData myFrameBufferData;
		SObjectBufferData myObjectBufferData;
		SSpriteBufferData mySpriteBufferData;

		ID3D11DeviceContext* myContext;
		AutoReleaser<ID3D11Buffer> myFrameBuffer;
		AutoReleaser<ID3D11Buffer> myObjectBuffer;
		AutoReleaser<ID3D11Buffer> mySpriteBuffer;

		ID3D11PixelShader* myFisheyePixelShader;
		ID3D11PixelShader* myCircularFillPixelShader;
		ID3D11PixelShader* mySpritePixelShader;
		ID3D11VertexShader* mySpriteVertexShader;
	};
}
