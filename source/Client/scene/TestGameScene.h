#pragma once

#include "FrameworkCore/FrameworkCore.h"
#include "Logger/Logger.h"
#include "GameCore/ClassicSimulation.h"

#include "Client/game/ControllablePlayer.h"

namespace Scene
{
	class TestGameScene
		:
		public Framework::Scene
	{
	public:
		bool onCreate() override
		{
			simulation.setPlayer();
			simulation.runWorld();

			return true;
		}

		void initialize() override
		{

		}

		void onEvent(const sf::Event event) override
		{
		}

		void onLogic(const sf::Time time) override
		{
		}

		void onDraw(sf::RenderTarget* const target) override
		{
		}

		void onShow() override
		{
		}

		void onHide() override
		{
		}

	private:
		Game::VisualClassicSimulation simulation;
	};
}
