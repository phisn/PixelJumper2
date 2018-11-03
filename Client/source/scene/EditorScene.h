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
		}

		void initialize() override
		{
			containers.push_back(
				new EDITOR::GridMenu(&world)
			);
		}

		void onShow() override { }
		void onHide() override { }
	private:
		EDITOR::World world;
	};
}
