#include "WinMenuSceneBase.h"

#include <iostream>

void Scene::WinMenuBase::onShow()
{
	// Ignore
}

void Scene::WinMenuBase::onHide()
{
	DestroyWindow(window);
}

void Scene::WinMenuBase::onEvent(
	sf::Event event)
{
	// Ignore
}

void Scene::WinMenuBase::onLogic(
	sf::Time time)
{
	// Ignore
}

void Scene::WinMenuBase::onDraw()
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

		if (message == WM_CLOSE && Scene::Handler::popScene())
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

Scene::WinMenuBase* Scene::WinMenuBase::currentMenu = NULL;