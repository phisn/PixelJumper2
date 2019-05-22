#pragma once

#include <Client/source/editor/selector/SelectorRoot.h>

#include <Client/source/menu/MenuButtonMaterial.h>
#include <Client/source/menu/MenuButtonWithLabel.h>
#include <Client/source/menu/MenuRootBase.h>
#include <Client/source/menu/MenuSlideBar.h>
#include <Client/source/menu/MenuScrollContainer.h>

#include <Client/source/menu/shape/CrossShape.h>

#include <Client/source/scene/MainSceneBase.h>

#include <iostream>

#include <Client/source/editor/manipulator/Manipulator.h>

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
			Editor::Manipulator::Initialize();
			sr = new Editor::SelectorRoot();
			sr->build();
			/*
			cs.position = { 100, 100 };
			cs.size = { 50, 50 };
			cs.consumption = 0.075f;

			root.addElement(&container);

			const float width = 0.2f;
			root.viewPort = { 1.f - width, 0.0f, width, 1.f };

			container.slideBarWidth = 30.f;
			container.size = root.size.getValue();
			
			buttons.reserve(20);

			for (int i = 0; i < 20; ++i)
			{
				buttons.emplace_back(i + 1);

				container.addElement(&buttons[i]);

				buttons[i].label.text = std::to_wstring(i + 1);
				buttons[i].size = { buttons[i].parent->space->get().x, 100.f };
			}
			*/

			return true;
		}

		void onRemove() override
		{
		}

		void initialize() override
		{
			// root.initialize();
			sr->initialize();
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
			sr->onEvent(event);
		}

		void onLogic(const sf::Time time) override
		{
		}

		void onDraw() override
		{
			sr->draw();
		}

	private:
		class Root : public Menu::MenuRootBase
		{
			bool build()
			{
				return true;
			}

		} root;
		
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

		Editor::SelectorRoot* sr;

		Menu::ScrollContainer<Menu::BarMaterial::DefaultRectangle, Menu::SliderMaterial::DefaultRectangle> container;
		std::vector<CButton> buttons;
	};
}