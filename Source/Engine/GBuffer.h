#pragma once
#include <d3d11.h>
#include <array>


namespace SE
{
	class CFullscreenTexture;
	class CGBuffer
	{
		friend class CTextureFactory;

	public:
		enum EGBufferTexture : uint
		{
			E_POSITION,
			E_ALBEDO,
			E_NORMAL,
			E_VERTEXNORMAL,
			E_MATERIAL,
			E_AMBIENTOCCLUSION,
			E_DEPTH,

			E_COUNT
		};

	private:
		std::array<ID3D11Texture2D*, EGBufferTexture::E_COUNT> myTextures;
		std::array<ID3D11RenderTargetView*, EGBufferTexture::E_COUNT> myRTVs;
		std::array<ID3D11ShaderResourceView*, EGBufferTexture::E_COUNT> mySRVs;
		D3D11_VIEWPORT* myViewport = nullptr;

	public:
		CGBuffer();
		~CGBuffer();

		void Release();
		void ClearTextures();
		void SetAsActiveTarget(CFullscreenTexture* aDepth = nullptr);
		void SetAsResourceOnSlot(EGBufferTexture aTexture, unsigned int aSlot);
		void SetAllAsResources(int aStartSlot = 2);
	};
}

