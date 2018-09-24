#pragma once

#include "SceneHandler.h"
#include "WinMenuSceneBase.h"

#include <Client/resource.h>

namespace Scene
{
	class DebugMenu
		:
		public WinMenuBase
	{
	public:
		DebugMenu(int counter)
			:
			WinMenuBase(IDD_DEBUG_DIALOG),
			counter(counter)
		{
		}

	private:
		INT_PTR windowProcess(
			HWND window, 
			UINT message, 
			WPARAM wparam, 
			LPARAM lparam) override;

		bool onCreate() override;
		bool onRemove() override;

		int counter;
	};
}
