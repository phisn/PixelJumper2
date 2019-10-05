#pragma once


#include <Client/source/game/GamemodeBase.h>

#include <Client/source/resource/ResourceInterface.h>
#include <Client/source/resource/WorldResource.h>

#include <Client/source/scene/MainSceneBase.h>

namespace Scene
{
	class TestReleaseGameScene
		:
		public MainSceneBase
	{
	public:
		void onRemove() override
		{
			delete world;
			delete testGamemode;
		}

		bool onCreate() override
		{
			Resource::World worldResource;

			if (!Resource::Interface::LoadResource(
					&worldResource,
					Resource::ResourceType::World,
					L"TestWorld"))
			{
				Log::Error(L"Failed to read resource");
				return false;
			}

			world = new Game::LocalWorld(&worldResource);
			testGamemode = new Game::TestGamemode(world);

			return testGamemode->initialize();
		}

		void initialize() override
		{
		}

		void onShow() override
		{
		}

		void onHide() override
		{
		}

		void onEvent(const sf::Event event) override
		{
			if (event.type == sf::Event::Closed)
			{
				Framework::Context::Pop();

				return;
			}

			testGamemode->onEvent(event);

			if (event.type == sf::Event::KeyPressed &&
				event.key.code == sf::Keyboard::Key::T &&
				event.key.control)
			{
				Framework::Context::Pop();
			}

			if (event.type == sf::Event::KeyPressed &&
				event.key.code == sf::Keyboard::Key::R)
			{
				testGamemode->world->getPlayers()[0]->getProperties().movement = { 0, 0 };
				testGamemode->world->getPlayers()[0]->getProperties().position = { 0, -1 };

				testGamemode->world->getPlayers()[0]->getProperties().update();
			}
		}

		void onLogic(const sf::Time time) override
		{
			testGamemode->onLogic(time);
		}

		void onDraw(sf::RenderTarget* const target) override
		{
			testGamemode->onDraw(target);
		}

	private:
		Game::LocalWorld* world;
		Game::TestGamemode* testGamemode;
	};
}
