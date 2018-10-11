#include "TestMenuScene.h"

INT_PTR Scene::TestMenu::windowProcess(
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
			Handler::pushScene( new TestMenu(counter + 1) );
		}

		break;
	case WM_INITDIALOG:
	{
		wchar_t c[] = { (wchar_t) (L'0' + counter), L'\0' };

		SetDlgItemText(
			window,
			IDC_TEST_COUNTER,
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

bool Scene::TestMenu::onCreate()
{
	return true;
}

bool Scene::TestMenu::onRemove()
{
	return true;
}
