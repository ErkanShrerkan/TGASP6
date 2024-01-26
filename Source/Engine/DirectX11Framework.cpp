#include "pch.h"
#include "DirectX11Framework.h"
#pragma comment(lib, "d3d11.lib")
#include <d3d11.h>
#include "WindowHandler.h"
#include "Engine.h"

#include <d3d11sdklayers.h>

#include "DX11.h"
#pragma comment(lib, "dxgi")

namespace SE
{
	CDirectX11Framework::CDirectX11Framework()
	{
		mySwapChain = nullptr;
		myDevice = nullptr;
		myContext = nullptr;
		myBackBuffer = nullptr;
		myDepthBuffer = nullptr;
	}

	CDirectX11Framework::~CDirectX11Framework()
	{
		if (mySwapChain != nullptr)
		{
			mySwapChain->SetFullscreenState(FALSE, NULL);
		}
	}

	bool CDirectX11Framework::CollectAdapters(uint2 aWindowSize, int2& aNumDenumerator, IDXGIAdapter** anOutAdapter)
	{
		aWindowSize; aNumDenumerator;
		HRESULT result;
		IDXGIFactory* factory;

		DXGI_MODE_DESC* displayModeList = nullptr;
		//unsigned int numModes = 0;
		//unsigned int i = 0;
		//unsigned int denominator = 0;
		//unsigned int numerator = 0;
		result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
		if (FAILED(result))
		{
			return false;
		}
		// Use the factory to create an adapter for the primary graphics interface (video card).
		IDXGIAdapter* usingAdapter = nullptr;
		int adapterIndex = 0;
		std::vector<DXGI_ADAPTER_DESC> myAdapterDescs;
		std::vector<IDXGIAdapter*> myAdapters;
		while (factory->EnumAdapters(adapterIndex, &usingAdapter) != DXGI_ERROR_NOT_FOUND)
		{
			DXGI_ADAPTER_DESC adapterDesc;
			usingAdapter->GetDesc(&adapterDesc);
			myAdapterDescs.push_back(adapterDesc);
			myAdapters.push_back(usingAdapter);
			++adapterIndex;
		}
		if (adapterIndex == 0)
		{
			return false;
		}

		DXGI_ADAPTER_DESC usingAdapterDesc;
		size_t mostMem = 0;

		#pragma warning(disable:4477)
		// 'printf_s' : format string ... requires an argument of type ...
		printf("----------------------------------------------------\n");
		printf_s("Found %i graphic cards\n", myAdapterDescs.size());
		for (unsigned int i = 0; i < myAdapterDescs.size(); i++)
		{

			DXGI_ADAPTER_DESC desc = myAdapterDescs[i];
			size_t memory = desc.DedicatedVideoMemory / 1024 / 1024;
			std::wstring name = desc.Description;

			#pragma warning(disable:4244)
			// 'argument' : conversion from 'wchar_t' to 'const _Elem', possible loss of data <xstring:2575>
			std::string cname = std::string(name.begin(), name.end());
			printf("- %s : %i MiB\n", cname.c_str(), memory);
			#pragma warning(default:4244)

			if (memory > mostMem)
			{
				mostMem = memory;
				usingAdapterDesc = desc;
				usingAdapter = myAdapters[i];
			}
		}
		{
			std::wstring name = usingAdapterDesc.Description;
			#pragma warning(disable:4244)
			// 'argument' : conversion from 'wchar_t' to 'const _Elem', possible loss of data <xstring:2575>
			std::string cname = std::string(name.begin(), name.end());
			printf("Using graphic card: %s\n", cname.c_str());
			#pragma warning(default:4244)
		}
		printf("----------------------------------------------------\n");
		#pragma warning(default:4477)

		//// Enumerate the primary adapter output (monitor).
		//IDXGIOutput* pOutput = nullptr;
		//if (usingAdapter->EnumOutputs(0, &pOutput) != DXGI_ERROR_NOT_FOUND)
		//{
		//	// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
		//	result = pOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
		//	if (!FAILED(result))
		//	{
		//		// Create a list to hold all the possible display modes for this monitor/video card combination.
		//		displayModeList = new DXGI_MODE_DESC[numModes];
		//		if (displayModeList)
		//		{
		//			// Now fill the display mode list structures.
		//			result = pOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
		//			if (!FAILED(result))
		//			{
		//				// Now go through all the display modes and find the one that matches the screen width and height.
		//				// When a match is found store the numerator and denominator of the refresh rate for that monitor.
		//				for (unsigned int i = 0; i < numModes; i++)
		//				{
		//					if (displayModeList[i].Width == (unsigned int)aWindowSize.x)
		//					{
		//						if (displayModeList[i].Height == (unsigned int)aWindowSize.y)
		//						{
		//							numerator = displayModeList[i].RefreshRate.Numerator;
		//							denominator = displayModeList[i].RefreshRate.Denominator;
		//						}
		//					}
		//				}
		//			}
		//		}
		//	}
		//	// Release the adapter output.
		//	pOutput->Release();
		//	pOutput = 0;
		//}

		// Get the adapter (video card) description.
		result = usingAdapter->GetDesc(&usingAdapterDesc);
		if (FAILED(result))
		{
			return false;
		}

		// Release the display mode list.
		delete[] displayModeList;
		displayModeList = 0;

		// Release the factory.
		factory->Release();
		factory = 0;

		*anOutAdapter = usingAdapter;
		return true;
	}

	bool CDirectX11Framework::Init(CWindowHandler* aWindowHandler)
	{
		DXGI_SWAP_CHAIN_DESC swapChainDescription = {};
		swapChainDescription.BufferCount = 1;
		swapChainDescription.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDescription.OutputWindow = aWindowHandler->GetWindowHandle();
		swapChainDescription.SampleDesc.Count = 1;	
		swapChainDescription.Windowed = true;

		int2 numDenum;
		IDXGIAdapter* adapter = nullptr;
		uint2 windowSize = { 2556, 1440 };
		if (!CollectAdapters(windowSize, numDenum, &adapter))
		{
			perr("<SE::CDirectX11Framework::Init> Error collecting adapters");
		}

		HRESULT result;
		result = D3D11CreateDeviceAndSwapChain(
			adapter, D3D_DRIVER_TYPE_UNKNOWN,
			nullptr, 0, nullptr, 0, D3D11_SDK_VERSION,
			&swapChainDescription, &mySwapChain, &myDevice, nullptr, &myContext);
		if (FAILED(result)) { /* Error message here */ return false; }

		ID3D11Texture2D* backbufferTexture;
		result = mySwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backbufferTexture));
		if (FAILED(result)) { /* Error message here */ return false; }

		result = myDevice->CreateRenderTargetView(backbufferTexture, nullptr, &myBackBuffer);
		if (FAILED(result)) { /* Error message here */ return false; }

		result = backbufferTexture->Release();
		if (FAILED(result)) { /* Error message here */ return false; }

		return true;
	}

	void CDirectX11Framework::BeginFrame(const Vector4f& aClearColor)
	{
		myContext->ClearRenderTargetView(myBackBuffer.Raw(), &aClearColor.x);
		//myContext->ClearDepthStencilView(myDepthBuffer.Raw(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	void CDirectX11Framework::EndFrame()
	{
		//mySwapChain->Present(1, 0); // vSync
		mySwapChain->Present(0, 0);
	}

	ID3D11DeviceContext *const& CDirectX11Framework::GetContext() const
	{
		return myContext.Raw();
	}

	ID3D11Device *const& CDirectX11Framework::GetDevice() const
	{
		return myDevice.Raw();
	}
	ID3D11RenderTargetView* const& CDirectX11Framework::GetBackBuffer() const
	{
		return myBackBuffer.Raw();
	}
}
