#pragma once
#include "WindowHandler.h"
#include "DirectX11Framework.h"
#include "RenderManager.h"
#include "ForwardRenderer.h"
#include "ImGuiInterface.h"

namespace SE
{
	class CGraphicsEngine
	{
	public:
		CGraphicsEngine();
		~CGraphicsEngine();

		bool Init(const CWindowHandler::SWindowData& someData);
		void BeginFrame();
		void RenderFrame();
		void EndFrame();

		CDirectX11Framework& GetFramework();
		CWindowHandler& GetWindowHandler();

		const Vector4f& GetClearColor() const;
		void SetClearColor(const Vector4f& aColor);

		CRenderManager::PostProcessingData& GetPostProcessingData();

	private:
		Vector4f myClearColor;
		CWindowHandler myHandler;

		CForwardRenderer myForwardRenderer;
		CRenderManager myRenderer;

		CDirectX11Framework myFramework;
		CImGuiInterface myImGuiInterface;
	};
}
