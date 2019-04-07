#pragma once

#include <Client/source/menu/MenuLabel.h>
#include <Client/source/menu/MenuButton.h>
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
			button.addStaticChild(&label);
			root.addElement(&button);

			label.text = L"Hello World";
			
			button.position = { 100, 100 };
			button.size = { 300, 150 };

			return true;
		}

		void onRemove() override
		{

		}

		void initialize() override
		{
			label.font = Framework::Interface::GetResource<sf::Font>(Resource::Static::DefaultFont);
			label.position = (button.size.getValue() - label.size.getValue()) / 2.f - sf::Vector2f(0.f, 8.f);

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
			root.onLogic(time);
		}

		void onDraw() override
		{
			root.onDraw();
		}

	private:
		Menu::RootBase root;
		Menu::Label label;
		
		class _Button : public Menu::Button<Menu::DefaultButtonMaterial>
		{
			void onButtonPressed() override
			{
				Log::Information(L"Button Pressed");
			}

			void onButtonHovered() override
			{
			}

			void onButtonDefault() override
			{
			}
		} button;
	};
}