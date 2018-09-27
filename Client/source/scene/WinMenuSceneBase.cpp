#include "WinMenuSceneBase.h"

#include <iostream>

namespace Scene
{
	void WinMenuBase::onShow()
	{
		// Ignore
	}

	void WinMenuBase::onHide()
	{
		DestroyWindow(window);
	}

	void WinMenuBase::onEvent(
		sf::Event event)
	{
		// Ignore
	}

	void WinMenuBase::onLogic(
		sf::Time time)
	{
		// Ignore
	}

	void WinMenuBase::onDraw(
		DEVICE::Window* window)
	{
		static HINSTANCE instance = GetModuleHandle(NULL);

		window = NULL;
		currentMenu = this;

		DialogBoxParamW(
			instance,
			MAKEINTRESOURCE(resourceId),
			NULL,
			[](HWND window,
				UINT message,
				WPARAM wparam,
				LPARAM lparam)
			-> INT_PTR
		{
			if (WinMenuBase::currentMenu->window == NULL)
			{
				WinMenuBase::currentMenu->window = window;
			}

			if (message == WM_CLOSE && Handler::popScene())
			{
				return NULL;
			}

			return WinMenuBase::currentMenu->windowProcess(
				window,
				message,
				wparam,
				lparam);
		}, 0);
	}

	WinMenuBase* WinMenuBase::currentMenu = NULL;
}
