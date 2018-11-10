#pragma once

#include <Client/source/device/DeviceInterface.h>
#include <Client/source/device/ScreenDevice.h>

#include <Client/source/editor/grid/GridMenu.h>
#include <Client/source/editor/manipulator/Manipulator.h>
#include <Client/source/editor/World.h>

#include <Client/source/scene/MainSceneBase.h>
#include <Client/source/scene/MenuBaseScene.h>

#include <SFML/Graphics.hpp>

namespace Scene
{
	/*
	
		Codebase information:
		-	Only add methods provided because there
			is ususally no need to remove things (static)
	
	*/

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
			for (MENU::RootBase* const container : getContainers())
			{
				delete container;
			}
		}

		void initialize() override
		{
			// initialized first
			EDITOR::Manipulator::initialize(&world);

			EDITOR::GridMenu* const gridMenu 
				= new EDITOR::GridMenu(&world);

			gridMenu->setViewport(
				{ 0.0f, 0.0f, 0.95f, 1.0f });

			addRoot(gridMenu);
		}

		void onShow() override { }
		void onHide() override { }
	private:
		EDITOR::World world;
	};
}