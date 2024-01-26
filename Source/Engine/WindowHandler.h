#pragma once
#include <windows.h>
#include <Game\Observer.h>

namespace SE
{
	class CWindowHandler : public Observer
	{
	public:
		struct SWindowData
		{
			int x, y, width, height;
			LPCWSTR title;
		};

		CWindowHandler();
		~CWindowHandler();

		void RecieveMessage(eMessage aMsg) override;
		bool Init(const SWindowData& someData);
		void InitSubscribtions();
		static LRESULT CALLBACK WinProc(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);

		HWND& GetWindowHandle();

		const int GetWidth() const;
		const int GetHeight() const;

	private:
		SWindowData myWindowData;
		HWND myWindowHandle;
	};
}
