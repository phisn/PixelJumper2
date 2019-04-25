#pragma once

#include <Client/source/menu/MenuButtonMaterial.h>
#include <Client/source/menu/MenuButtonWithLabel.h>
#include <Client/source/menu/MenuRootBase.h>
#include <Client/source/menu/MenuSlideBar.h>
#include <Client/source/menu/MenuScrollContainer.h>

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
			slideBar(Menu::CommonControlDirection::Horizontal),
			container(Menu::CommonControlDirection::Horizontal)
		{
		}

		bool onCreate() override
		{
			root.addElement(&container);

			button.size = { 300, 200 };
			button.label.text = L"Button1";

			button2.size = { 200, 300 };
			button.label.text = L"Button2";

			container.addElement(&button);
			container.addElement(&label2);
			container.addElement(&button2);

			container.position = { 0, 0 };
			container.size = { 400, 400 };

			label2.text = L"Hello";

			root.viewPort = root.convertToPortRect(
				container.position,
				container.size.getValue());

			return true;
		}

		void onRemove() override
		{

		}

		void initialize() override
		{
			root.initialize();
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
			static sf::Time old;
			static bool world = false;

			if ((old += time) > sf::milliseconds(500))
			{
				if (world = !world)
				{
					label2.text = L"World";
				}
				else
				{
					label2.text = L"Hello";
				}

				old = sf::microseconds(0);
			}

			root.onLogic(time);
		}

		void onDraw() override
		{
			root.onDraw();
		}

	private:
		Menu::RootBase root;
		
		class : public Menu::ButtonWithLabel<Menu::ButtonMaterial::DefaultCircle>
		{
		public:
			void onButtonPressed() override
			{
				Log::Information(L"Button Pressed");
			}

		} button;
		
		class : public Menu::ButtonWithLabel<Menu::ButtonMaterial::DefaultRectangleStaticSize>
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

		Menu::ScrollContainer<Menu::BarMaterial::DefaultRectangle, Menu::SliderMaterial::DefaultRectangle> container;
		Menu::Label label, label2;
	};
}