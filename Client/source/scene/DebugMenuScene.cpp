#include "DebugMenuScene.h"

namespace Scene
{
	bool DebugMenu::onCreate()
	{
		return true;
	}

	bool DebugMenu::onRemove()
	{
		return true;
	}

	INT_PTR DebugMenu::windowProcess(
		HWND window,
		UINT message,
		WPARAM wparam,
		LPARAM lparam)
	{

		switch (message)
		{
		case WM_COMMAND:
			if (LOWORD(wparam) == IDC_DEBUG_RUN_SP)
			{
				Handler::pushScene( createSinglePlayerScene() );
			}

			break;
		default:
			return DefWindowProc(
				window,
				message,
				wparam,
				lparam);
		}

		return NULL;
	}

	Base* DebugMenu::createSinglePlayerScene()
	{
		return new PixelJumper(&context);
	}
}
