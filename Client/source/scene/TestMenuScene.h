#pragma once

#include <Client/source/editor/selector/SelectorRoot.h>
#include <Client/source/editor/tilechoice/TileChoiceRoot.h>
#include <Client/source/editor/tilechoice/TileSearchRoot.h>

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
		{
		}

		bool onCreate() override
		{
			Editor::Manipulator::Initialize();
			
			sr = new Editor::SelectorRoot();
			tcr = new Editor::TileChoiceRoot();
			tsr = new Editor::TileSearchRoot();


			sr->build();
			tcr->build();
			tsr->build();

			sr->viewPort = { 0.f, 0.f, 0.7f, 1.f };
			tcr->viewPort = { 0.7f, 0.15f, 0.3f, 0.85f };
			tsr->viewPort = { 0.7f, 0.f, 0.3f, 0.15f };

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
			tcr->initialize();
			tsr->initialize();
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
			tcr->onEvent(event);
			tsr->onEvent(event);
		}

		void onLogic(const sf::Time time) override
		{
		}

		void onDraw() override
		{
			sr->draw();
			tcr->draw();
			tsr->draw();
		}

	private:
		Editor::SelectorRoot* sr;
		Editor::TileChoiceRoot* tcr;
		Editor::TileSearchRoot* tsr;
	};
}