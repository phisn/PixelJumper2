#pragma once

#include <Client/source/game/GamemodeBase.h>

#include <Client/source/resource/ResourceInterface.h>
#include <Client/source/resource/WorldResource.h>

#include <Client/source/scene/MainSceneBase.h>

#include <Client/source/framework/FrameworkInterface.h>


namespace Scene
{
	class TestGameScene
		:
		public MainSceneBase
	{
	public:
		TestGameScene(Resource::World* const world = NULL)
			:
			world(world),
			testGamemode(&this->world)
		{
		}

		bool onCreate() override
		{
			return testGamemode.initialize();
		}

		void onRemove() override
		{
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

		void onScenePopped(const int size) override
		{
		}

		void onEvent(const sf::Event event) override
		{
			if (event.type == sf::Event::Closed)
			{
				Framework::Context::Pop();

				return;
			}

			testGamemode.onEvent(event);

			if (event.type == sf::Event::KeyPressed &&
				event.key.code == sf::Keyboard::Key::T &&
				event.key.control)
			{
				Framework::Context::Pop();
			}
		}

		void onLogic(const sf::Time time) override
		{
			testGamemode.onLogic(time);
		}

		void onDraw(sf::RenderTarget* const target) override
		{
			testGamemode.onDraw(target);
		}

	private:
		Game::World world;
		Game::TestGamemode testGamemode;
	};
}
