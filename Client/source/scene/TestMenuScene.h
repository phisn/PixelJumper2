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
			container(Menu::CommonControlDirection::Vertical)
		{
		}

		bool onCreate() override
		{
			root.addElement(&container);

			root.viewPort = { 0.7f, 0.0f, 0.3f, 1.f };

			container.position = { 0, 0 };
			
			buttons.reserve(100);

			for (int i = 0; i < 100; ++i)
			{
				buttons.emplace_back(i + 1);

				buttons[i].label.text = std::to_wstring(i + 1);
				buttons[i].size = { 100.0f, 65.0f };

				container.addElement(&buttons[i]);
			}

			sf::FloatRect rect = root.convertToPortRect(
				{ 0, 0 },
				container.size);

			rect.left = 1.f - rect.width;
			rect.height = 1.f;

			root.viewPort = rect;

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
		}

		void onDraw() override
		{
			root.onDraw();
		}

	private:
		Menu::RootBase root;
		
		class CButton : public Menu::ButtonWithLabel<Menu::ButtonMaterial::DefaultRectangleStaticSize>
		{
		public:
			CButton(int i)
				:
				i(i)
			{
			}

			void onButtonPressed() override
			{
				Log::Information(L"Button Pressed: " + std::to_wstring(i));
			}

		private:
			int i;
		};

		Menu::ScrollContainer<Menu::BarMaterial::DefaultRectangle, Menu::SliderMaterial::DefaultRectangle> container;
		std::vector<CButton> buttons;
	};
}