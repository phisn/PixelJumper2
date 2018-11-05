#pragma once

#include <Client/source/device/DeviceInterface.h>
#include <Client/source/device/ScreenDevice.h>

#include <Client/source/editor/grid/GridMenu.h>
#include <Client/source/editor/World.h>

#include <Client/source/scene/MainSceneBase.h>
#include <Client/source/scene/MenuBaseScene.h>

#include <SFML/Graphics.hpp>

namespace Scene
{
	class Editor
		:
		public MenuBase
	{
	public:
		bool onCreate() override
		{
			return true;
		}

		void onRemove() override
		{
			for (MENU::RootBase* const container : containers)
			{
				delete container;
			}
		}

		void initialize() override
		{
			EDITOR::GridMenu* const gridMenu = new EDITOR::GridMenu(&world);

			gridMenu->setViewport(
				{ 0.0f, 0.0f, 0.95f, 1.0f });

			containers.push_back(gridMenu);
		}

		void onShow() override { }
		void onHide() override { }
	private:
		EDITOR::World world;
	};
}
