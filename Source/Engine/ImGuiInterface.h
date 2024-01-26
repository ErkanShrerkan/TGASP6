#pragma once
#include <Windows.h>
struct ID3D11Device;
struct ID3D11DeviceContext;
namespace SE
{
	class CImGuiInterface
	{
	public:
		~CImGuiInterface();
		bool Init(HWND, ID3D11Device*, ID3D11DeviceContext*);
		void ApplyDefaultStyle();
		void BeginFrame();
		void EndFrame();
	};
}
