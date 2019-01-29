#pragma once

#include <Client/source/device/DeviceInterface.h>
#include <Client/source/device/RandomDevice.h>
#include <Client/source/device/ScreenDevice.h>

#include <Client/source/editor/grid/GridMenu.h>
#include <Client/source/editor/manipulator/Manipulator.h>
#include <Client/source/editor/tilemenu/TileMenu.h>
#include <Client/source/editor/EditorWorld.h>

#include <Client/source/resource/ResourceInterface.h>
#include <Client/source/resource/WorldResource.h>

#include <Client/source/scene/MenuBaseScene.h>

#include <random>

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
		{
		}

		bool onCreate() override
		{
			EDITOR::Manipulator::Initialize();

			EDITOR::GridMenu* gridMenu = addRoot<EDITOR::GridMenu>();
			gridMenu->setViewport({ 0.0f, 0.0f, 0.9f, 1.0f });
			if (!gridMenu->initialize())
			{
				return false;
			}

			EDITOR::TileMenu* tileMenu = addRoot<EDITOR::TileMenu>();
			tileMenu->setViewport({ 0.9f, 0.0f, 0.1f, 1.0f });
			if (!tileMenu->initialize())
			{ // move to onCreate?
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

		void onEvent(const sf::Event event) override
		{
			MenuBase::onEvent(event);

			if (event.type == sf::Event::KeyPressed &&
				event.key.code == sf::Keyboard::S &&
				event.key.control)
			{
				Log::Section section(L"Saving world");

				Resource::World* world = EDITOR::Manipulator::GetWorld()->convert(
					Device::Random::MakeRandom<int>(),
					L"Author Name",
					L"Map Name");

				if (world == NULL)
				{
					Log::Error(L"Failed to create world");

					return;
				}

				if (Resource::Interface::WriteResource(
						world, 
						Resource::ResourceType::World, 
						L"TestWorld")
					)
				{
					/*
					
						Tiles are not correctly saved
					
					*/

					Log::Information(L"Successfully saved world");
				}
				else
				{
					Log::Error(L"Failed to save world");
				}

				delete world;
			}
		}

		void initialize() override 
		{
		}

		void onShow() override { }
		void onHide() override { }
	private:
	};
}
