#pragma once

#include <Client/source/game/GamemodeBase.h>

#include <Client/source/resource/ResourceInterface.h>
#include <Client/source/resource/WorldResource.h>

#include <Client/source/scene/MenuBaseScene.h>

#include <Client/source/menu/MenuLabel.h>
#include <Client/source/menu/MenuSlideBar.h>
#include <Client/source/menu/MenuRowContainer.h>

#include <Client/source/shared/tiles/TileWall.h>>

#include <Client/source/framework/FrameworkInterface.h>


namespace Scene
{
	class GameSlider
		:
		public Menu::RowContainer
	{
	public:
		GameSlider(
			const std::wstring name,
			Menu::Property<float>& p)
			:
			p(p),
			RowContainer(Menu::CommonControlDirection::Horizontal),
			slider(Menu::CommonControlDirection::Horizontal)
		{
			this->name.text = name;
		}

		void build()
		{
			name.sizePreferred = { 75, 20 };
			slider.sizePreferred = { 350, 20 };
			value.sizePreferred = { 75, 20 };

			sizePreferred = { 500, 20 };
			size = { 500, 20 };

			name.space.automatic = sf::Vector2<bool>(false, false);
			slider.space.automatic = sf::Vector2<bool>(false, false);
			value.space.automatic = sf::Vector2<bool>(false, false);

			addElement(&name);
			addElement(&slider);
			addElement(&value);

			value.text = L"0.000000";

			slider.length->setPercent(0.1f);
			slider.sliderMoved.addListener(
				[this](const float distance)
				{
					const float result = convertDistanceToValue(slider.distance->getPercent());

					value.text = std::to_wstring(result).substr(0, 8);
					p.setValue(result);
				});
		}

	private:
		float convertDistanceToValue(const float distance)
		{
			const float fm = fmodf(distance, 0.1f);
			return (powf(10, (distance - fm) * 10 - 5) * fm * 128);
		}

		Menu::Property<float>& p;
		Menu::SlideBar<Menu::BarMaterial::DefaultRectangle, Menu::SliderMaterial::DefaultRectangle> slider;
		Menu::Label name, value;
	};

	class GameSlidersRoot
		:
		public Menu::MenuRootBase
	{
	public:
		GameSlidersRoot(Game::TestGamemode* const testGamemode)
			:
			testGamemode(testGamemode),
			container(Menu::CommonControlDirection::Vertical)
		{
		}

		~GameSlidersRoot()
		{
		}

		bool build() override
		{
			player = testGamemode->world->getPlayers()[0];

			addChild(&container);

			slider.push_back(new GameSlider(std::wstring(L"Speed"), player->getProperties().speed));
			container.addElement(slider.back());
			slider.back()->build();
			slider.push_back(new GameSlider(std::wstring(L"Friction"), player->getProperties().friction));
			container.addElement(slider.back());
			slider.back()->build();
			slider.push_back(new GameSlider(std::wstring(L"Weight"), player->getProperties().weight));
			container.addElement(slider.back());
			slider.back()->build();
			slider.push_back(new GameSlider(std::wstring(L"AirResistance"), testGamemode->world->getProperties().airResistance));
			container.addElement(slider.back());
			slider.back()->build();

			gravityProperty.addListener(
				[this](const float, const float gravity)
				{
					testGamemode->world->getProperties().gravity = sf::Vector2f(0, gravity);
				});

			slider.push_back(new GameSlider(std::wstring(L"Gravity Y"), gravityProperty));
			container.addElement(slider.back());
			slider.back()->build();

			tileDensityProperty.addListener(
				[this](const float, const float density)
				{
					for (Game::WallTile* tile : testGamemode->world->getEnvironment()->getTileType<Game::WallTile>())
					{
						tile->setDND(density, tile->getFriction());
					}
				});

			slider.push_back(new GameSlider(std::wstring(L"Tile Density"), tileDensityProperty));
			container.addElement(slider.back());
			slider.back()->build();

			
			slider.push_back(new GameSlider(std::wstring(L"Tile Friction"), gravityProperty));
			container.addElement(slider.back());
			slider.back()->build();

			return true;
		}

	private:
		Menu::RowContainer container;
		std::vector<GameSlider*> slider;

		Game::Property<float> gravityProperty;
		Game::Property<float> tileDensityProperty;
		Game::Property<float> tileFrictionProperty;

		float convertDistanceToValue(const float distance)
		{
			const float fm = fmodf(distance, 0.1f);
			return powf(10, (distance - fm) * 10 - 5) * fm * 100;
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

			sf::FloatRect viewPort = Menu::MenuRootBase::ConvertRealToPort({ 0, 0, 500, 0 });
			viewPort.height = 1.f;

			addRoot<GameSlidersRoot>(&testGamemode)->viewPort = viewPort;
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

			if (event.type == sf::Event::KeyPressed &&
				event.key.code == sf::Keyboard::Key::R)
			{
				testGamemode.world->getPlayers()[0]->getProperties().movement = { 0, 0 };
				testGamemode.world->getPlayers()[0]->getProperties().position = { 0, -1 };
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
