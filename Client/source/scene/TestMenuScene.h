#pragma once

#include <Client/source/editor/selector/SelectorRoot.h>
#include <Client/source/editor/tilechoice/TileChoiceRoot.h>
#include <Client/source/editor/tilechoice/TileSearchRoot.h>

#include <Client/source/scene/MenuBaseScene.h>

#include <Client/source/editor/manipulator/Manipulator.h>

#include <Client/source/menu/MenuLabel.h>

namespace Scene
{
	class TestMenuScene
		:
		public MenuBaseScene
	{
	public:
		virtual bool onCreate() override
		{
			Editor::Manipulator::Initialize();

			Editor::SelectorRoot* const sr = addRoot<Editor::SelectorRoot>();
			Editor::TileChoiceRoot* const tcr = addRoot<Editor::TileChoiceRoot>();
			Editor::TileSearchRoot* const tsr = addRoot<Editor::TileSearchRoot>();

			sr->viewPort = { 0.f, 0.f, 0.7f, 1.f };
			tcr->viewPort = { 0.7f, 0.15f, 0.3f, 0.85f };
			tsr->viewPort = { 0.7f, 0.f, 0.3f, 0.15f };

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

		Menu::Label label;
	};
}