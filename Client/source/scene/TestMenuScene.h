#pragma once

#include <Client/source/menu/MenuLabel.h>
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
			label.text = L"Hello World";
			label.position = { 100, 100 };

			root.addElement(&label);
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

		}

		void onLogic(const sf::Time time) override
		{

		}

		void onDraw() override
		{
			root.onDraw();
			sf::Font::GetD
		}

	private:
		Menu::RootBase root;
		Menu::Label label;
	};
}