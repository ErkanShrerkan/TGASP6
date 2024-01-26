#pragma once
#include "VFXManager.h"

struct ID3D11BlendState;
struct ID3D11Buffer;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
namespace SE
{
	class CCamera;
	class CVFXRenderer
	{
	public:
		bool Init();
		void Render(CCamera* aCamera, std::vector<CVFXManager::SVFXCollection>& someVFXs);

	private:

		struct SFrameBufferData
		{
			Matrix4x4f myCameraTransform;
			Matrix4x4f myToCamera;
			Matrix4x4f myToProjection;
			Vector4f myCameraPosition;
		} myFrameBufferData;

		struct SObjectBufferData
		{
			Matrix4x4f myToWorld;
			float4 myColor;
			float3 myScale;
			int myBlend;
			float2 myUV;
			float2 trash2;
		} myObjectBufferData;

		ID3D11Buffer* myFrameBuffer;
		ID3D11Buffer* myObjectBuffer;

		ID3D11PixelShader* myPixelShader;
		ID3D11VertexShader* myVertexShader;
	};

}
