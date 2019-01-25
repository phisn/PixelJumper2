#pragma once

#include <Client/source/device/DeviceInterface.h>
#include <Client/source/device/ScreenDevice.h>

#include <Client/source/editor/grid/GridMenu.h>
#include <Client/source/editor/tilemenu/TileMenu.h>

#include <Client/source/editor/manipulator/Manipulator.h>
#include <Client/source/editor/World.h>

#include <Client/source/scene/MainSceneBase.h>
#include <Client/source/scene/MenuBaseScene.h>

#include <SFML/Graphics.hpp>

#include <windows.h>

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
		Editor()
			:
			world()
		{
		}

		bool onCreate() override
		{
			EDITOR::Manipulator::initialize(&world);

			EDITOR::GridMenu* gridMenu = addRoot<EDITOR::GridMenu>(&world);
			gridMenu->setViewport({ 0.0f, 0.0f, 0.9f, 1.0f });
			if (!gridMenu->initialize())
			{ // move to onCreate?
				onFailedInitialize();

				return false;
			}

			EDITOR::TileMenu* tileMenu = addRoot<EDITOR::TileMenu>();
			tileMenu->setViewport({ 0.9f, 0.0f, 0.1f, 1.0f });
			if (!tileMenu->initialize())
			{ // move to onCreate?
				onFailedInitialize();

				return false;
			}

			return true;
		}

		void onDraw() override
		{
			MenuBase::onDraw();
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
		}

		void onShow() override { }
		void onHide() override { }
	private:
		void onFailedInitialize()
		{
			MessageBox(NULL, L"Failed", L"Error", MB_OK);
		}

		// TODO: make accessable to tasks
		::Editor::World world;
	};
}
