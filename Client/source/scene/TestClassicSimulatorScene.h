#pragma once

#include <Client/source/game/HostClassicSimulator.h>
#include <Client/source/scene/MainSceneBase.h>

namespace Scene
{
	class TestClassicSimulatorScene
		:
		public MainSceneBase
	{
	public:
		bool onCreate() override
		{
			return simulator.initialize();
		}

		void onRemove() override
		{

		}

		void initialize() override
		{
		}

		void onShow() override
		{
		}

		void onHide() override
		{
		}

		void onEvent(const sf::Event event) override
		{
		}

		void onLogic(const sf::Time time) override
		{
			simulator.onLogic(time);
		}

		void onDraw(sf::RenderTarget* const target) override
		{
		}
		
	private:
		Game::Net::HostClassicSimulator simulator;
	};
}
