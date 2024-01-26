#include "pch.h"
#include "FullscreenTexture.h"
#pragma comment(lib, "d3d11.lib")
#include <d3d11.h>
#include "Engine.h"

namespace SE
{
	CFullscreenTexture::~CFullscreenTexture()
	{
		/*if (myViewport)
		{
			delete myViewport;
			myViewport = nullptr;
		}*/
	}

	void CFullscreenTexture::Release()
	{
		if (myTexture)
		{
			myTexture->Release();
			myRenderTargetView->Release();
		}
		if (myShaderResourceView)
		{
			myShaderResourceView->Release();
		}
	}

	void CFullscreenTexture::ClearTexture(const Vector4f& aClearColor)
	{
		myContext->ClearRenderTargetView(myRenderTargetView, &aClearColor.x);
	}

	void CFullscreenTexture::ClearDepth(float aClearDepthValue, uint aClearStencilValue)
	{
		UINT8 clearStencilValue = static_cast<UINT8>(aClearStencilValue);
		myContext->ClearDepthStencilView(myDepth, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, aClearDepthValue, clearStencilValue);
	}

	void CFullscreenTexture::SetAsActiveTarget(int anIndex, CFullscreenTexture* aDepth)
	{
		if (aDepth)
		{
			myContext->OMSetRenderTargets(anIndex, &myRenderTargetView, aDepth->myDepth);
		}
		else
		{
			myContext->OMSetRenderTargets(anIndex, &myRenderTargetView, nullptr);
		}

		myContext->RSSetViewports(1, myViewport.get());
	}

	void CFullscreenTexture::SetAsResourceOnSlot(uint aSlot)
	{
		myContext->PSSetShaderResources(aSlot, 1, &myShaderResourceView);
	}

	void CFullscreenTexture::SetDeviceContext(ID3D11DeviceContext* aContext)
	{
		myContext = aContext;
	}
}
