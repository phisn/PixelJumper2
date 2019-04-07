#pragma once

#include <Client/source/menu/MenuLabel.h>
#include <Client/source/menu/MenuButton.h>
#include <Client/source/menu/MenuButtonWithLabel.h>
#include <Client/source/menu/MenuRootBase.h>

#include <Client/source/scene/MainSceneBase.h>

namespace Scene
{
	class TestMenuScene
		:
		public MainSceneBase
	{
	public:
		TestMenuScene()
		{
		}

		bool onCreate() override
		{
			root.addElement(&button);

			button.label.text = L"Hello World";
			
			button.position = { 100, 100 };
			button.size = { 500, 250 };

			return true;
		}

		void onRemove() override
		{

		}

		void initialize() override
		{
			button.label.font = Framework::Interface::GetResource<sf::Font>(Resource::Static::DefaultFont);
			
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
			static sf::Time counter;

			counter += time;

			if ((int) counter.asMilliseconds() % 1000 > 500)
			{
				button.label.text = L"World";
			}
			else
			{
				button.label.text = L"Hello";
			}

			root.onLogic(time);
		}

		void onDraw() override
		{
			root.onDraw();
		}

	private:
		Menu::RootBase root;
		
		class _Button : public Menu::ButtonWithLabel<Menu::ButtonMaterial::DefaultRectangle>
		{
			void onButtonPressed() override
			{
				Log::Information(L"Button Pressed");
			}

		} button;
	};
}