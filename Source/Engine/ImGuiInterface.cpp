#include "pch.h"
#include "ImGuiInterface.h"
#include <Windows.h>

// ImGui
#include <ImGui\imgui.h>
#include <ImGui\imgui_impl_win32.h>
#include <ImGui\imgui_impl_dx11.h>

namespace SE
{
	// Easy HEX to normalised RGB values
	template <int hex>
	struct HexColor
	{
		const ImVec4 value = {
			static_cast<float>((hex >> 24) & 0xFF) / 255.f,
			static_cast<float>((hex >> 16) & 0xFF) / 255.f,
			static_cast<float>((hex >> 8) & 0xFF) / 255.f,
			static_cast<float>((hex >> 0) & 0xFF) / 255.f,
		};
	};

	CImGuiInterface::~CImGuiInterface()
	{
		// Cleanup
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	bool CImGuiInterface::Init(HWND hwnd, ID3D11Device* aDXDevice, ID3D11DeviceContext* aDXDeviceContext)
	{
		//IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		(void)io;

		io.IniFilename = nullptr;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking

		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX11_Init(aDXDevice, aDXDeviceContext);

		ApplyDefaultStyle();

		return true;
	}

	void CImGuiInterface::ApplyDefaultStyle()
	{
		const ImVec4 imgcNone
		(HexColor<0x00000000>().value);

		const ImVec4 imgcTextColor
		(HexColor<0xFFFFFFFF>().value);
		const ImVec4 imgcDisabledTextColor
		(HexColor<0xFFFFFF80>().value);
		const ImVec4 imgcSelectedTextColor
		(HexColor<0x8952C880>().value);

		const ImVec4 imgcTitleBG
		(HexColor<0x3D3D3DFF>().value);
		const ImVec4 imgcBorder
		(HexColor<0x303030FF>().value);
		const ImVec4 imgcMenuBarBG
		(HexColor<0x202020FF>().value);
		const ImVec4 imgcWindowBackground
		(HexColor<0x181818FF>().value);
		const ImVec4 imgcFrameBG
		(HexColor<0x121212FF>().value);

		const ImVec4 imgcActive
		(HexColor<0xBB7BB8FF>().value);
		const ImVec4 imgcHover
		(HexColor<0x8952C8FF>().value);

		const ImVec4 imgcUnknown
		(HexColor<0xFFFF0080>().value);

		ImGuiStyle* style = &ImGui::GetStyle();
		ImVec4* colors = style->Colors;
		colors[ImGuiCol_Text] = imgcTextColor;
		colors[ImGuiCol_TextDisabled] = imgcDisabledTextColor;
		colors[ImGuiCol_WindowBg] = imgcWindowBackground;
		colors[ImGuiCol_ChildBg] = imgcNone;
		colors[ImGuiCol_PopupBg] = imgcWindowBackground;
		colors[ImGuiCol_Border] = imgcBorder;
		colors[ImGuiCol_BorderShadow] = imgcNone;
		colors[ImGuiCol_FrameBg] = imgcFrameBG;
		colors[ImGuiCol_FrameBgHovered] = imgcFrameBG;
		colors[ImGuiCol_FrameBgActive] = imgcFrameBG;
		colors[ImGuiCol_TitleBg] = imgcTitleBG;
		colors[ImGuiCol_TitleBgActive] = imgcTitleBG;
		colors[ImGuiCol_TitleBgCollapsed] = imgcTitleBG;
		colors[ImGuiCol_MenuBarBg] = imgcMenuBarBG;
		colors[ImGuiCol_ScrollbarBg] = imgcNone;
		colors[ImGuiCol_ScrollbarGrab] = imgcMenuBarBG;
		colors[ImGuiCol_ScrollbarGrabHovered] = imgcTitleBG;
		colors[ImGuiCol_ScrollbarGrabActive] = imgcTitleBG;
		colors[ImGuiCol_CheckMark] = imgcHover;
		colors[ImGuiCol_SliderGrab] = imgcBorder;
		colors[ImGuiCol_SliderGrabActive] = imgcHover;
		colors[ImGuiCol_Button] = imgcFrameBG;
		colors[ImGuiCol_ButtonHovered] = imgcHover;
		colors[ImGuiCol_ButtonActive] = imgcActive;
		colors[ImGuiCol_Header] = imgcMenuBarBG;
		colors[ImGuiCol_HeaderHovered] = imgcTitleBG;
		colors[ImGuiCol_HeaderActive] = imgcTitleBG;
		colors[ImGuiCol_Separator] = imgcBorder;
		colors[ImGuiCol_SeparatorHovered] = imgcHover;//Still unknown//ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
		colors[ImGuiCol_SeparatorActive] = imgcHover;
		colors[ImGuiCol_ResizeGrip] = imgcTitleBG;
		colors[ImGuiCol_ResizeGripHovered] = imgcHover;
		colors[ImGuiCol_ResizeGripActive] = imgcActive;
		colors[ImGuiCol_Tab] = imgcMenuBarBG;
		colors[ImGuiCol_TabHovered] = imgcTitleBG;
		colors[ImGuiCol_TabActive] = imgcHover;
		colors[ImGuiCol_TabUnfocused] = imgcMenuBarBG;//ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
		colors[ImGuiCol_TabUnfocusedActive] = imgcMenuBarBG;//ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
		colors[ImGuiCol_DockingPreview] = imgcHover;
		colors[ImGuiCol_DockingEmptyBg] = imgcWindowBackground;
		colors[ImGuiCol_PlotLines] = imgcHover;
		colors[ImGuiCol_PlotLinesHovered] = imgcActive;
		colors[ImGuiCol_PlotHistogram] = imgcHover;
		colors[ImGuiCol_PlotHistogramHovered] = imgcActive;
		colors[ImGuiCol_TableHeaderBg] = imgcMenuBarBG;
		colors[ImGuiCol_TableBorderStrong] = imgcBorder;
		colors[ImGuiCol_TableBorderLight] = imgcBorder;
		colors[ImGuiCol_TableRowBg] = imgcWindowBackground;
		colors[ImGuiCol_TableRowBgAlt] = imgcFrameBG;
		colors[ImGuiCol_TextSelectedBg] = imgcSelectedTextColor;
		colors[ImGuiCol_DragDropTarget] = imgcUnknown;//ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavHighlight] = imgcUnknown;//ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = imgcUnknown;//ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = imgcUnknown;//ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = imgcUnknown;//ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

		style->FrameBorderSize = 1.f;

		style->ChildRounding = 3.f;
		style->PopupRounding = 3.f;

		style->WindowRounding = 3.f;
		style->FrameRounding = 3.f;
		style->GrabRounding = 2.f;
		style->GrabMinSize = 8.f;
		style->ScrollbarRounding = 2.f;
		style->ScrollbarSize = 12.f;
	}

	void CImGuiInterface::BeginFrame()
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void CImGuiInterface::EndFrame()
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
}
