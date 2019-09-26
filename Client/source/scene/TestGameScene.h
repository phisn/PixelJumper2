#pragma once

#include <Client/source/game/GamemodeBase.h>

#include <Client/source/resource/ResourceInterface.h>
#include <Client/source/resource/WorldResource.h>

#include <Client/source/scene/MenuBaseScene.h>

#include <Client/source/menu/MenuSlideBar.h>
#include <Client/source/menu/MenuRowContainer.h>

#include <Client/source/framework/FrameworkInterface.h>


namespace Scene
{
	class GameSlidersRoot
		:
		public Menu::MenuRootBase
	{
	public:
		GameSlidersRoot(Game::TestGamemode* const testGamemode)
			:
			testGamemode(testGamemode),

			container(Menu::CommonControlDirection::Vertical),
			speed(Menu::CommonControlDirection::Horizontal)
		{
		}

		bool build() override
		{
			player = testGamemode->world->getPlayers()[0];

			addChild(&container);
			container.addChild(&speed);

			container.sizePreferred.setX(500);

			speed.sizePreferred.setY(20);
			speed.length->setPercent(0.1f);

			speed.sliderMoved.addListener(
				[this](const float distance)
				{
					player->getProperties().speed = convertDistanceToValue(speed.distance->getPercent()) * 0.001f;
					Log::Information(std::to_wstring(speed.distance->getPercent() * 0.001f));
				});

			return true;
		}

	private:
		Menu::RowContainer container;
		Menu::SlideBar<Menu::BarMaterial::DefaultRectangle, Menu::SliderMaterial::DefaultRectangle> speed;

		float convertDistanceToValue(const float distance)
		{
			if (distance > 0.5f) // result below 1.f
			{

			}
			else // result above 1.f
			{

			}
			return distance;
		}

		Game::PlayerBase* player;
		Game::TestGamemode* const testGamemode;
	};

	class TestGameScene
		:
		public MenuBaseScene
	{
	public:
		TestGameScene(Resource::World* const world)
			:
			world(world),
			testGamemode(&this->world)
		{
		}

		bool onCreate() override
		{
			if (!testGamemode.initialize())
			{
				return false;
			}

			addRoot<GameSlidersRoot>(&testGamemode)->viewPort = { 0.f, 0.f, 0.4f, 1.f };
			return MenuBaseScene::onCreate();
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

			MenuBaseScene::onEvent(event);
		}

		void onLogic(const sf::Time time) override
		{
			testGamemode.onLogic(time);
			MenuBaseScene::onLogic(time);
		}

		void onDraw(sf::RenderTarget* const target) override
		{
			testGamemode.onDraw(target);
			MenuBaseScene::onDraw(target);
		}

	private:
		Game::World world;
		Game::TestGamemode testGamemode;
	};
}
