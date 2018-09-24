#include "DebugMenuScene.h"

INT_PTR Scene::DebugMenu::windowProcess(
	HWND window, 
	UINT message, 
	WPARAM wparam, 
	LPARAM lparam)
{
	switch (message)
	{
	case WM_COMMAND:
		if (LOWORD(wparam) == IDC_DEBUG_TEST)
		{
			Handler::pushScene( new DebugMenu(counter + 1) );
		}

		break;
	case WM_INITDIALOG:
	{
		wchar_t c[] = { (wchar_t) (L'0' + counter), L'\0' };

		SetDlgItemText(
			window,
			IDC_DEBUG_COUNTER,
			(LPCWSTR) c);

		break;
	}
	default:
		return DefWindowProc(
			window,
			message,
			wparam,
			lparam);
	}
	
	return NULL;
}

bool Scene::DebugMenu::onCreate()
{
	return true;
}

bool Scene::DebugMenu::onRemove()
{
	return true;
}
