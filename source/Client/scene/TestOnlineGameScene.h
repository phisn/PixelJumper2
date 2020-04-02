#pragma once

#include "game/ClientClassicConnection.h"

#include "FrameworkCore/FrameworkCore.h"
#include "FrameworkCore/ViewChain.h"

#include <imgui/imgui.h>

namespace Scene
{
	class TestGameScene
		:
		public Framework::Scene
	{
	public:
		TestGameScene()
		{
		}

		bool onCreate() override
		{
		}

		void initialize() override
		{
		}

		void onEvent(const sf::Event event) override
		{
			if (event.type == sf::Event::Resized)
			{
				viewChain.remakeViewViewports();
			}
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
		Game::ClientClassicConnection connection;
	};
}
