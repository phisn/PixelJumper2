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

			label.position = { 0, 0 };
			label.text = L"0";
			label.color = sf::Color::Yellow;


			return MenuBaseScene::onCreate() && label.create();
		}

		virtual void onScenePopped(const int size) override
		{
		}

		void initialize() override
		{
			MenuBaseScene::initialize();

			label.initialize();
		}

		void onLogic(const sf::Time time) override
		{
			sum -= timeArray[timeCounter];
			sum += time;

			timeArray[timeCounter] = time;

			(++timeCounter) %= arraySize;
			MenuBaseScene::onLogic(time);
		}

		void onDraw() override
		{
			label.text = std::to_wstring((int) std::round(1.0 / (sum.asMicroseconds() / (arraySize * 1'000'000.0))));

			MenuBaseScene::onDraw();

			label.onDraw();
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

		Menu::Label label;
	};
}