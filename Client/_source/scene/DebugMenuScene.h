#pragma once

#include <Client/resource.h>
#include <Client/source/scene/WinMenuSceneBase.h>

#include <Client/source/PixelJumper/PixelJumperContext.h>
#include <Client/source/scene/PixelJumperScene.h>

namespace Scene
{
	class DebugMenu
		:
		public WinMenuBase
	{
	public:
		DebugMenu()
			:
			WinMenuBase(IDD_DEBUG_MENU)
		{
		}

		bool onCreate() override;
		bool onRemove() override;
	private:
		INT_PTR windowProcess(
			HWND window, 
			UINT message, 
			WPARAM wparam, 
			LPARAM lparam) override;

		Base* createSinglePlayerScene();
		PJ::Context context;
	};
}
