#pragma once
#include <d3d11.h>
#include <memory>
#include <Engine\Scene.h>

class Camera;

using namespace CommonUtilities;

namespace SE
{
	class CShadowRenderer
	{
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
		} myObjectBufferData;

		ID3D11Buffer* myObjectBuffer;
		ID3D11Buffer* myFrameBuffer;
		ID3D11Buffer* myShadowFrameBuffer;
		ID3D11PixelShader* myPixelShader;

	public:
		CShadowRenderer();
		~CShadowRenderer();

		bool Init();
		void Render(std::shared_ptr<CCamera> aCamera, const std::vector<ModelCollectionExtended>& someModels, int anIndex = 0);
	};
}

