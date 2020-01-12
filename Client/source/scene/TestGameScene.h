#pragma once

#include <Client/source/game/GamemodeBase.h>

#include <Client/source/resource/ResourceInterface.h>
#include <Client/source/resource/WorldResource.h>

#include <Client/source/scene/MenuBaseScene.h>

#include <Client/source/menu/MenuLabel.h>
#include <Client/source/menu/MenuSlideBar.h>
#include <Client/source/menu/MenuRowContainer.h>

#include <Client/source/shared/tiles/TileWall.h>

#include <Client/source/framework/FrameworkInterface.h>

namespace Scene
{
	/*
	class GameSlider
		:
		public Menu::RowContainer
	{
	public:
		GameSlider(
			const std::wstring name,
			Menu::Property<float>& gameProperty,
			const float default)
			:
			gameProperty(gameProperty),
			defaultValue(default),

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

			// bug fix begin TODO: fix cause later
			// size = { 500, 20 };

			name.space.automatic = sf::Vector2<bool>(false, false);
			slider.space.automatic = sf::Vector2<bool>(false, false);
			value.space.automatic = sf::Vector2<bool>(false, false);
			// bug fix end

			addElement(&name);
			addElement(&slider);
			addElement(&value);

			slider.length->setPercent(0.1f);
			slider.sliderMoved.addListener(
				[this](const float distance)
				{
					const float result = convertDistanceToValue(slider.distance->getPercent());

					value.text = std::to_wstring(result).substr(0, 8);
					gameProperty.setValue(result);
					
				});

			slider.resetValue.addListener(
				[this]()
				{
					resetDefaultValue();
				});

			resetDefaultValue();
		}

	private:
		float convertDistanceToValue(const float distance)
		{
			const float fm = fmodf(distance, 0.1f);
			const float fc = fm == 0 ? 1 : (fm * 128);

			return (powf(10, (distance - fm) * 10 - 5) * fc);
		}

		void resetDefaultValue()
		{
			const int percent_e = (int) log10f(defaultValue);
			const float percent = (percent_e + 5) / 10.f + (defaultValue / powf(10, percent_e)) / 128.f;

			slider.distance->setPercent(percent);
		}

		float defaultValue;
		Menu::Property<float>& gameProperty;

		class CustomSlider : public Menu::SlideBar<Menu::BarMaterial::DefaultRectangle, Menu::SliderMaterial::DefaultRectangle>
		{
		public:
			CustomSlider(Menu::CommonControlDirection d)
				:
				SlideBar(d)
			{
			}

			void onEvent(const sf::Event event) override
			{
				SlideBar::onEvent(event);

				if (*weakSelected && event.type == sf::Event::KeyPressed)
				{
					if (event.key.code == sf::Keyboard::Left)
					{
						distance->setValue(distance->getValue() - 0.025f);

						return;
					}

					if (event.key.code == sf::Keyboard::Right)
					{
						distance->setValue(distance->getValue() + 0.025f);


						return;
					}

					if (event.key.code == sf::Keyboard::E)
					{
						resetValue.notify();

						return;
					}
				}
			}

			Menu::MenuNotifier<CustomSlider> resetValue;

		} slider;

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

			slider.push_back(new GameSlider(std::wstring(L"Input Force"), player->getProperties().inputForce, testGamemode->world->getInformation()->defaultPlayerProperties.inputForce));
			container.addElement(slider.back());
			slider.back()->build();
			slider.push_back(new GameSlider(std::wstring(L"Input Reduce"), player->getProperties().inputReduce, testGamemode->world->getInformation()->defaultPlayerProperties.inputReduce));
			container.addElement(slider.back());
			slider.back()->build();
			slider.push_back(new GameSlider(std::wstring(L"Mass"), player->getProperties().mass, testGamemode->world->getInformation()->defaultPlayerProperties.mass));
			container.addElement(slider.back());
			slider.back()->build();
			slider.push_back(new GameSlider(std::wstring(L"AirResistance"), testGamemode->world->getProperties().airResistance, testGamemode->world->getInformation()->defaultWorldProperties.airResistance));
			container.addElement(slider.back());
			slider.back()->build();

			gravityProperty.addListener(
				[this](const float, const float gravity)
				{
					testGamemode->world->getProperties().gravity = sf::Vector2f(0, gravity);
				});

			slider.push_back(new GameSlider(std::wstring(L"Gravity Y"), gravityProperty, testGamemode->world->getInformation()->defaultWorldProperties.gravity.y));
			container.addElement(slider.back());
			slider.back()->build();

			tileDensityProperty.addListener(
				[this](const float, const float density)
				{
					for (Game::CollidableTile* const tile : testGamemode->world->getEnvironment()->getTileType<Game::CollidableTile>())
					{
						tile->setDensity(density);
					}
				});

			slider.push_back(new GameSlider(std::wstring(L"Tile Density"), tileDensityProperty, 1.f));
			container.addElement(slider.back());
			slider.back()->build();

			tileFrictionProperty.addListener(
				[this](const float, const float value)
				{
					for (Game::CollidableTile* const tile : testGamemode->world->getEnvironment()->getTileType<Game::CollidableTile>())
					{
						tile->setFriction(value);
					}
				});
			
			slider.push_back(new GameSlider(std::wstring(L"Tile Friction"), tileFrictionProperty, 1.f));
			container.addElement(slider.back());
			slider.back()->build();

			tileForceProperty.addListener(
				[this](const float, const float value)
				{
					for (Game::CollidableTile* const tile : testGamemode->world->getEnvironment()->getTileType<Game::CollidableTile>())
					{
						tile->setForce(value);
					}
				});

			slider.push_back(new GameSlider(std::wstring(L"Tile InputForce"), tileForceProperty, 1.f));
			container.addElement(slider.back());
			slider.back()->build();

			 viewSizeProperty.addListener(
				[this](const float, const float value)
				{
					 testGamemode->world->getPlayers()[0]->getProperties().viewWindow.setValue({ 0.f, 0.f, value, value });
				});

			slider.push_back(new GameSlider(std::wstring(L"View Size"), viewSizeProperty, 20.f));
			container.addElement(slider.back());
			slider.back()->build();

			return true;
		}

	private:
		Menu::RowContainer container;
		std::vector<GameSlider*> slider;

		Menu::Property<float> gravityProperty;
		Menu::Property<float> tileDensityProperty;
		Menu::Property<float> tileFrictionProperty;
		Menu::Property<float> tileCriticalProperty;
		Menu::Property<float> tileForceProperty;
		Menu::Property<float> viewSizeProperty;

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

				testGamemode.world->getPlayers()[0]->getProperties().update();
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
		Game::LocalWorld world;
		Game::TestGamemode testGamemode;
	};
	*/
}
