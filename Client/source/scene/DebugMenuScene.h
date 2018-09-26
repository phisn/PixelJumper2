#pragma once

#include <Client/resource.h>
#include <Client/source/scene/WinMenuSceneBase.h>

#include <Client/source/scene/PixelJumper/PixelJumperContext.h>
#include <Client/source/scene/PixelJumper/PixelJumperScene.h>

namespace Scene
{
	class DebugMenu
		:
		public WinMenuBase
	{
	public:
		DebugMenu()
			:
			WinMenuBase(IDD_DEBUG_DIALOG)
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
		::PixelJumper::Context context;
	};
}
