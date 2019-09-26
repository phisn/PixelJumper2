#pragma once

#include <Client/source/game/UserConnection.h>
#include <Client/source/game/GameWorld.h>

#include <Client/source/game/tiletrait/ExitableTile.h>

#include <Client/source/framework/Context.h>

#include <vector>

namespace Game
{
	class GamemodeBase
	{
	public:
		virtual bool initialize() = 0;

		virtual void onLogic(const sf::Time time) = 0;
		virtual void onEvent(const sf::Event event) = 0;
		virtual void onDraw(sf::RenderTarget* const) = 0;
	};

	class ClassicGamemode
	{
	public:
		ClassicGamemode()
		{
		}

	private:
		World* world;
	};

	class SoloGamemode
		:
		public GamemodeBase
	{
	public:
		World world;
		LocalConnection player;
	};

	class TestGamemode
		:
		public GamemodeBase
	{
		PlayerInformation playerInfo{ sf::Color::Red, L"Test Player" };

	public:
		TestGamemode(World* const world)
			:
			world(world),
			user(playerInfo, Device::Input::PlayerId::P1, { 0, 0, 1, 1 })
		{
		}

		~TestGamemode()
		{
		}

		bool initialize() override
		{
			if (!world->initialize())
			{
				return false;
			}

			user.createPlayer(world);

			for (ExitableTile* tile : world->getEnvironment()->getTileType<ExitableTile>())
			{
				tile->onExit.addListener(
				[this]()
				{
					Framework::Context::Pop();
				});
			}

			return true;
		}

		void onLogic(const sf::Time time) override
		{
			world->onLogic(time);
		}

		void onEvent(const sf::Event event)
		{ // reserved for use in future
		}

		void onDraw(sf::RenderTarget* const target)
		{
			user.enableView(target);
			world->draw(target);
			Device::Screen::ResetView();
		}

	// private:
		LocalConnection user;
		World* const world;
	};
}
