#pragma once

#include <Client/source/scene/SceneBase.h>
#include <Client/source/scene/LocalGameScene.h>

#include <Windows.h>

namespace Scene
{
	class Debug
		:
		public Base
	{
	public:
		Debug()
			:
			Base(false, Scene::Type::Windows)
		{
		}

		bool onCreate() override { }

		void onRemove() override { }

		void onShow() override 
		{
			pushSinglePlayer(2);
		}
		void onHide() override { }

		void onEvent(
			sf::Event event) override { }
		void onLogic(
			sf::Time time) override { }
		void onDraw() override  { }
	private:
		bool pushSinglePlayer(int playerCount)
		{
			LocalGameSettings settings;

			settings.playerCount = playerCount;

			settings.worldSettings = { };
			settings.worldSettings.begin = { 0.f, 0.f };

			settings.playerSettings.emplace_back();
			settings.playerSettings.back().color = sf::Color::Red;
			settings.playerSettings.back().name = L"Player 1";

			settings.playerSettings.emplace_back();
			settings.playerSettings.back().color = sf::Color::Red;
			settings.playerSettings.back().name = L"Player 2";

			if (!Interface::pushScene(
				new LocalGame(&settings)
			))
			{
				MessageBoxW(
					NULL,
					L"Failed to load game",
					L"Error",
					MB_OK);

				return false;
			}
		}
	};
}
