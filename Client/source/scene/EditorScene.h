#pragma once

#include <Client/source/device/RandomDevice.h>
#include <Client/source/device/ScreenDevice.h>

#include <Client/source/editor/EditorWorld.h>
#include <Client/source/editor/grid/GridMenu.h>
#include <Client/source/editor/manipulator/Manipulator.h>
#include <Client/source/editor/manipulator/tasks/LoadWorldTask.h>
#include <Client/source/editor/subscene/GroupedTilePreview.h>
#include <Client/source/editor/tilemenu/TileMenu.h>

#include <Client/source/framework/FrameworkInterface.h>

#include <Client/source/resource/ResourceInterface.h>
#include <Client/source/resource/WorldResource.h>

#include <Client/source/scene/MenuBaseScene.h>

namespace Scene
{
	/*
	
		Codebase information:
		-	Only add methods provided because there
			is ususally no need to remove things (static)
	
	*/

	class EditorScene
		:
		public MenuBase
	{
	public:
		EditorScene()
		{
		}

		void onScenePopped(const int size) override
		{
			if (size == 0 && !running)
			{
				running = true;
			}
		}

		bool onCreate() override
		{
			Editor::Manipulator::Initialize();

			Editor::GridMenu* gridMenu = addRoot<Editor::GridMenu>();
			gridMenu->setViewport({ 0.0f, 0.0f, 0.9f, 1.0f });
			if (!gridMenu->initialize())
			{
				return false;
			}

			Editor::TileMenu* tileMenu = addRoot<Editor::TileMenu>();
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
			
			// TODO: normally resets view, make it obsolete
			for (sf::RectangleShape& shape : test)
			{
				Device::Screen::Draw(shape);
			}
		}

		void onRemove() override
		{
			for (Menu::RootBase* const container : getContainers())
			{
				delete container;
			}
		}

		void onEvent(const sf::Event event) override
		{
			MenuBase::onEvent(event);

			// save world | S + STRG
			if (event.type == sf::Event::KeyPressed &&
				event.key.code == sf::Keyboard::S &&
				event.key.control)
			{ // propably does not need task
				Log::Section section(L"Saving world");

				Resource::World* world = Editor::Manipulator::GetWorld()->convert(
					Device::Random::MakeRandom<int>(),
					L"Author Name",
					L"Map Name");

				if (world == NULL)
				{
					Log::Error(L"Failed to create world");

					return;
				}

				if (Resource::Interface::SaveResource(
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
			
			// load world | L + STRG
			if (event.type == sf::Event::KeyPressed &&
				event.key.code == sf::Keyboard::L &&
				event.key.control)
			{
				Editor::Manipulator::GetExecutor()->execute(
					new Editor::LoadWorldTask(L"TestWorld")
				);
			}

			// undo task | Z + STRG
			if (event.type == sf::Event::KeyPressed &&
				event.key.code == sf::Keyboard::Z &&
				event.key.control)
			{
				Editor::Manipulator::GetExecutor()->undo();
			}

			// redo task | Y + STRG
			if (event.type == sf::Event::KeyPressed &&
				event.key.code == sf::Keyboard::Y &&
				event.key.control)
			{
				Editor::Manipulator::GetExecutor()->redo();
			}

			if (event.type == sf::Event::KeyPressed &&
				event.key.code == sf::Keyboard::G &&
				event.key.control)
			{
				if (!Framework::Interface::PushScene(
						new Editor::GroupedTilePreview()
					))
				{
					Log::Error(L"Unable to create GroupedTilePreview");
				}

				this->running = false;
			}

			if (event.type == sf::Event::KeyPressed &&
				event.key.code == sf::Keyboard::Return)
			{
				Editor::Manipulator::GetExecutor()->execute<Editor::TilePlace>();
			}
		}

		void initialize() override 
		{
		}

		void onShow() override { }
		void onHide() override { }
	private:
		std::vector<sf::RectangleShape> test;
	};
}
