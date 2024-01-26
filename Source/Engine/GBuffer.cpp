#include "pch.h"
#include "GBuffer.h"
#include "FullscreenTexture.h"
#include "Engine.h"

namespace SE
{
	CGBuffer::CGBuffer()
	{
	}

	CGBuffer::~CGBuffer()
	{
	}

	void CGBuffer::Release()
	{
		for (int idx = 0; idx < CGBuffer::E_COUNT; idx++)
		{
			if (myTextures[idx])
			{
				myTextures[idx]->Release();
				myRTVs[idx]->Release();
				mySRVs[idx]->Release();
			}
		}

		delete myViewport;
	}

	void CGBuffer::ClearTextures()
	{
		float color[4] = { 0, 0, 0, 0 };
		for (int idx = 0; idx < CGBuffer::E_COUNT; idx++)
		{
			CEngine::GetInstance()->GetDXDeviceContext()->ClearRenderTargetView(myRTVs[idx], &color[0]);
		}
	}

	void CGBuffer::SetAsActiveTarget(CFullscreenTexture* aDepth)
	{
		ID3D11DeviceContext* const& context = CEngine::GetInstance()->GetDXDeviceContext();
		if (aDepth)
		{
			context->OMSetRenderTargets(EGBufferTexture::E_COUNT, &myRTVs[0], aDepth->myDepth);
		}
		else
		{
			context->OMSetRenderTargets(EGBufferTexture::E_COUNT, &myRTVs[0], nullptr);
		}

		context->RSSetViewports(1, myViewport);
	}

	void CGBuffer::SetAsResourceOnSlot(EGBufferTexture aTexture, unsigned int aSlot)
	{
		CEngine::GetInstance()->GetDXDeviceContext()->PSSetShaderResources(aSlot, 1, &mySRVs[aTexture]);
	}

	void CGBuffer::SetAllAsResources(int aStartSlot)
	{
		CEngine::GetInstance()->GetDXDeviceContext()->PSSetShaderResources(aStartSlot, EGBufferTexture::E_COUNT, &mySRVs[0]);
	}
}
