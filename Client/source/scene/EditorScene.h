#pragma once

#include <Client/source/editor/selector/SelectorRoot.h>
#include <Client/source/editor/tilechoice/TileChoiceRoot.h>
#include <Client/source/editor/tilechoice/TileSearchRoot.h>

#include <Client/source/editor/manipulator/Manipulator.h>
#include <Client/source/editor/manipulator/Executor.h>

#include <Client/source/editor/manipulator/tasks/LoadWorldTask.h>
#include <Client/source/editor/manipulator/tasks/TilePlace.h>

#include <Client/source/editor/subscene/GroupedTilePreview.h>

#include <Client/source/menu/MenuLabel.h>
#include <Client/source/menu/MenuFPSLabel.h>

#include <Client/source/scene/MenuBaseScene.h>
#include <Client/source/scene/TestGameScene.h>

namespace Scene
{
	class EditorScene
		:
		public MenuBaseScene
	{
	public:
		virtual bool onCreate() override
		{
			Editor::Manipulator::Initialize();

			/*
			
				add label for information, independent of and in center (bit up) on screen
				needs to have time to vanish (maybe even with increasing alpha) and to be
				automatically hidden after pressed (cursor or maybe even key like enter / escape)

			*/

			Editor::SelectorRoot* const sr = addRoot<Editor::SelectorRoot>();
			Editor::TileChoiceRoot* const tcr = addRoot<Editor::TileChoiceRoot>();
			Editor::TileSearchRoot* const tsr = addRoot<Editor::TileSearchRoot>();

			sr->viewPort = { 0.f, 0.f, 0.8f, 1.f };
			tcr->viewPort = { 0.8f, 0.05f, 0.2f, 0.95f };
			tsr->viewPort = { 0.8f, 0.f, 0.2f, 0.05f };

			fpsLabel.position = { 0, 0 };
			fpsLabel.color = sf::Color::Yellow;

			return MenuBaseScene::onCreate() && fpsLabel.create();
		}

		void initialize() override
		{
			MenuBaseScene::initialize();
			fpsLabel.initialize();
		}

		void onEvent(const sf::Event event) override
		{
			if (event.type == sf::Event::Closed)
			{
				Framework::Context::Pop();

				return;
			}

			MenuBaseScene::onEvent(event);

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
				if (!Framework::Context::PushScene(
						new Editor::GroupedTilePreview()
					))
				{
					Log::Error(L"Unable to create GroupedTilePreview");
				}
			}

			if (event.type == sf::Event::KeyPressed &&
				event.key.code == sf::Keyboard::R &&
				event.key.control)
			{
				if (Editor::Manipulator::GetWorld()->getTiles().size() == 0)
				{
					return;
				}

				Resource::World* const world = Editor::Manipulator::GetWorld()->convert(
					Device::Random::MakeRandom<int>(),
					L"Author Name",
					L"Map Name");

				if (world == NULL)
				{
					Log::Error(L"Failed to create world");
					return;
				}

				if (!world->setup())
				{
					Log::Error(L"Invalid world");

					delete world;
					return;
				}


				Framework::Context::Push<TestGameScene>(world);
			}

			if (event.type == sf::Event::KeyPressed &&
				event.key.code == sf::Keyboard::Return)
			{
				Editor::Manipulator::GetExecutor()->execute<Editor::TilePlace>();
			}
		}

		void onLogic(const sf::Time time) override
		{
			MenuBaseScene::onLogic(time);
			// fpsLabel.onLogic(time);
		}

		void onDraw(sf::RenderTarget* const target) override
		{
			MenuBaseScene::onDraw(target);
			fpsLabel.onDraw(target);
		}

	private:
		Menu::FPSLabel<> fpsLabel;
	};
}
