#pragma once

#include "SceneBase.h"
#include "SceneHandler.h"

#include <Windows.h>

namespace Scene
{
	class WinMenuBase
		:
		public Base
	{
	public:
		WinMenuBase(
			const int resourceId)
			:
			Base(Type::Win),
			resourceId(resourceId)
		{
		}

		void onShow() override;
		void onHide() override;

		void onEvent(
			sf::Event event) override;
		void onLogic(
			sf::Time time) override;
		void onDraw() override;
	protected:
		virtual INT_PTR windowProcess(
			HWND window,
			UINT message,
			WPARAM wparam,
			LPARAM lparam) = 0;

		HWND window = NULL;
	private:
		const int resourceId;
		static WinMenuBase* currentMenu;
	};
}
