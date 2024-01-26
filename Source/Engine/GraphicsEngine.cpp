#include "pch.h"
#include "GraphicsEngine.h"

#include <array>
#include "Engine.h"
#include "Scene.h"

#include <pix3.h>

namespace SE
{
	CGraphicsEngine::CGraphicsEngine()
	{
	}

	CGraphicsEngine::~CGraphicsEngine()
	{
	}

	bool CGraphicsEngine::Init(const CWindowHandler::SWindowData& someData)
	{
		if (!myHandler.Init(someData))
		{
			/* Error Message */
			return false;
		}

		if (!myFramework.Init(&myHandler))
		{
			/* Error Message */
			return false;
		}

		if (!myRenderer.Init(&myFramework))
		{
			/* Error Message */
			return false;
		}

		if (!myImGuiInterface.Init(GetWindowHandler().GetWindowHandle(), GetFramework().GetDevice(), GetFramework().GetContext()))
		{
			/* Error Message */
			return false;
		}

		myClearColor = { 1.0f, .85f, .66f, 1.f };

		return true;
	}

	void CGraphicsEngine::BeginFrame()
	{
		PIXBeginEvent(PIX_COLOR_INDEX(1), __FUNCTION__);
		myFramework.BeginFrame(myClearColor);
		myImGuiInterface.BeginFrame();
		PIXEndEvent();
	}

	void CGraphicsEngine::RenderFrame()
	{
		PIXBeginEvent(PIX_COLOR_INDEX(1), __FUNCTION__);
		myRenderer.RenderFrame();
		PIXEndEvent();
	}

	void CGraphicsEngine::EndFrame()
	{
		PIXBeginEvent(PIX_COLOR_INDEX(1), __FUNCTION__);
		myImGuiInterface.EndFrame();
		myFramework.EndFrame();
		PIXEndEvent();
	}

	CDirectX11Framework& CGraphicsEngine::GetFramework()
	{
		return myFramework;
	}

	CWindowHandler& CGraphicsEngine::GetWindowHandler()
	{
		return myHandler;
	}

	const Vector4f& CGraphicsEngine::GetClearColor() const
	{
		return myClearColor;
	}

	void CGraphicsEngine::SetClearColor(const Vector4f& aColor)
	{
		myClearColor = aColor;
	}
	CRenderManager::PostProcessingData& CGraphicsEngine::GetPostProcessingData()
	{
		return myRenderer.GetPPD();
	}
}
