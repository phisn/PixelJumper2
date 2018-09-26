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
			if (LOWORD(wparam) == IDC_TEST_PUSH)
			{
				Handler::pushScene(createSinglePlayerScene());
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

	Scene::Base* DebugMenu::createSinglePlayerScene()
	{
		return new PixelJumper(&context);
	}
}
