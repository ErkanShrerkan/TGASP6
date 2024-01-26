#pragma once

#include "AutoReleaser.h"

struct IDXGISwapChain;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct IDXGIAdapter;
struct IDXGIFactory;
namespace SE
{
	class CWindowHandler;
	class CDirectX11Framework
	{
	public:

		CDirectX11Framework();
		~CDirectX11Framework();
		bool CollectAdapters(uint2 aWindowSize, int2& aNumDenumerator, IDXGIAdapter** anOutAdapter);
		bool Init(CWindowHandler* aWindowHandler);

		void BeginFrame(const Vector4f& aClearColor);
		void EndFrame();

		ID3D11DeviceContext* const& GetContext() const;
		ID3D11Device* const& GetDevice() const;
		ID3D11RenderTargetView* const& GetBackBuffer() const;

	private:

		AutoReleaser<IDXGISwapChain> mySwapChain;
		AutoReleaser<ID3D11Device> myDevice;
		AutoReleaser<ID3D11DeviceContext> myContext;
		AutoReleaser<ID3D11RenderTargetView> myBackBuffer;
		AutoReleaser<ID3D11DepthStencilView> myDepthBuffer;
	};
}
