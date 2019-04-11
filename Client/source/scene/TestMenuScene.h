#pragma once

#include <Client/source/menu/MenuButtonMaterial.h>
#include <Client/source/menu/MenuButtonWithLabel.h>
#include <Client/source/menu/MenuRootBase.h>
#include <Client/source/menu/MenuSlideBar.h>

#include <Client/source/scene/MainSceneBase.h>

namespace Scene
{
	class TestMenuScene
		:
		public MainSceneBase
	{
	public:
		TestMenuScene()
			:
			slideBar(Menu::CommonControlDirection::Vertical)
		{
		}

		bool onCreate() override
		{
			root.addElement(&button);
			root.addElement(&button2);

			root.addElement(&slideBar);

			button.label.text = L"Hello World";
			
			button.position = { 100, 100 };
			button.size = { 300, 150 };

			button2.label.text = L"Hello World";

			button2.position = { 100, 300 };
			button2.size = { 300, 150 };

			slideBar.position = { 500, 100 };
			slideBar.size = { 30, 500 };

			return true;
		}

		void onRemove() override
		{

		}

		void initialize() override
		{
			button.label.font = Framework::Interface::GetResource<sf::Font>(Resource::Static::DefaultFont);
			button2.label.font = Framework::Interface::GetResource<sf::Font>(Resource::Static::DefaultFont);

			root.initialize();

			slideBar.length->setPercent(0.5f);
			slideBar.length.valueChanged(slideBar.length.getValue());
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
			root.onEvent(event);
		}

		void onLogic(const sf::Time time) override
		{
			static sf::Time counter;
			static bool isWorld = true;

			counter += time;

			if ((int) counter.asMilliseconds() % 1200 > 600)
			{
				if (!isWorld)
				{
						button.label.text = L"World";
						isWorld = true;
				}
			}
			else if (isWorld)
			{
				button.label.text = L"Hello";
				isWorld = false;
			}

			root.onLogic(time);
		}

		void onDraw() override
		{
			root.onDraw();
		}

	private:
		Menu::RootBase root;
		
		class : public Menu::ButtonWithLabel<Menu::ButtonMaterial::DefaultRectangleStaticSize>
		{
		public:
			void onButtonPressed() override
			{
				Log::Information(L"Button Pressed");
			}

		} button;
		
		class : public Menu::ButtonWithLabel<Menu::ButtonMaterial::DefaultRectangle>
		{
		public:
			void onButtonPressed() override
			{
				Log::Information(L"Button Pressed");
			}

		} button2;

		class : public Menu::SlideBar<Menu::BarMaterial::DefaultRectangle, Menu::SliderMaterial::DefaultRectangle>
		{
		public:
			using SlideBar::SlideBar;

			void onSliderPressed(const float distance) override
			{
			}

			void onSliderMoved(const float distance) override
			{
			}
		} slideBar;
	};
}