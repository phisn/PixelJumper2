#pragma once

#include <Client/source/editor/selector/SelectorRoot.h>
#include <Client/source/editor/tilechoice/TileChoiceRoot.h>
#include <Client/source/editor/tilechoice/TileSearchRoot.h>

#include <Client/source/scene/MenuBaseScene.h>

#include <Client/source/editor/manipulator/Manipulator.h>

#include <Client/source/menu/MenuLabel.h>
#include <Client/source/menu/MenuTextBox.h>

namespace Scene
{
	class TestMenuScene
		:
		public MenuBaseScene
	{
	public:
		virtual bool onCreate() override
		{
			addRoot<Root>();

			value.position = { 0, 0 };
			value.text = L"0";
			value.color = sf::Color::Yellow;


			return MenuBaseScene::onCreate() && value.create();
		}

		virtual void onScenePopped(const int size) override
		{
		}

		void initialize() override
		{
			MenuBaseScene::initialize();

			value.initialize();
		}

		void onLogic(const sf::Time time) override
		{
			sum -= timeArray[timeCounter];
			sum += time;

			timeArray[timeCounter] = time;

			(++timeCounter) %= arraySize;
			MenuBaseScene::onLogic(time);
		}

		void onDraw(sf::RenderTarget* const target) override
		{
			value.text = std::to_wstring((int) std::round(1.0 / (sum.asMicroseconds() / (arraySize * 1'000'000.0))));

			MenuBaseScene::onDraw(target);
			value.onDraw(target);
		}

	private:
		constexpr static int arraySize{ 20 };

		sf::Time sum{ };
		sf::Time timeArray[arraySize] = { };
		size_t timeCounter = 0;

		class Root : public Menu::MenuRootBase
		{
		public:
			bool build() override
			{
				tb.sizePreferred = sf::Vector2f(100, 40);
				tb.position = sf::Vector2f(200, 200);
				tb.text = L"- Input -";

				addChild(&tb);

				return true;
			}
			
		private:
			Menu::TextBox tb;

		};

		Menu::Label value;
	};
}