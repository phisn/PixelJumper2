#pragma once

#include <Client/source/game/UserConnection.h>
#include <Client/source/game/GameWorld.h>

#include <Client/source/game/tiletrait/ExitableTile.h>

#include <Client/source/framework/Context.h>

#include <vector>

namespace Game
{
	// gamemode must be able to sync players
	// gamemode should only sync internal gamemode
	// values, not dependent on the creator
	class GamemodeBase
		:
		public GameState
	{
	public:
		virtual bool initialize() = 0;
		virtual void processLogic() = 0;
	};

	class GamemodeCreatorBase
	{
	public:
		virtual GamemodeBase* createGamemode() = 0;
	};

	class ClassicGamemode
		:
		public GamemodeBase
	{
	public:
		typedef std::vector<Resource::World*> WorldResources;

		ClassicGamemode(const WorldResources& worldResources)
			:
			worldResources(worldResources)
		{
		}

		bool initialize()
		{
			// or preload all worlds?
			loadWorld(worldResources[0]);

			return currentWorld->initialize();
		}

		void processLogic()
		{
			if (requestWorldSwitch)
			{
				// load next world;
			}
		}

		void registerPlayer(PlayerBase* const player)
		{
		}

		World* getCurrentWorld() const
		{
			return currentWorld;
		}

		bool writeState(Resource::WritePipe* const writePipe)
		{
		}

		bool readState(Resource::ReadPipe* const readPipe)
		{
		}

	private:
		void loadWorld(Resource::World* const worldResource)
		{
			if (currentWorld != NULL)
			{
				delete currentWorld;
			}

			currentWorld = new World(worldResource);
			currentWorld->addPlayer(player);
		}

		void registerExitTiles()
		{
			for (ExitableTile* tile : currentWorld->getEnvironment()->getTileType<ExitableTile>())
			{
				tile->onExit.addListener(
					[this]()
					{
						requestWorldSwitch = true;
					});
			}
		}

		PlayerBase* const player;

		bool requestWorldSwitch = false;
		World* currentWorld = NULL;
		const WorldResources& worldResources;
	};

	class ClassicGamemodeCreator
		:
		public GamemodeCreatorBase
	{
	public:
		GamemodeBase* createGamemode() override
		{
			return new ClassicGamemode(worlds);
		}

	private:
		std::vector<Resource::World*> worlds;
	};

	class SoloGamemode
		:
		public GamemodeBase
	{
	public:
		LocalWorld world;
		LocalConnection player;
	};

	class TestGamemode
		:
		public GamemodeBase
	{
		PlayerInformation playerInfo{ sf::Color::Red, L"Test Player" };

	public:
		TestGamemode(LocalWorld* const world)
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
						if (!popingContext)
						{
							Framework::Context::Pop();
							popingContext = true;
						}
					});
			}

			return true;
		}

		void onLogic(const sf::Time time) override
		{
			world->onLogic(time);
		}

		void onEvent(const sf::Event event) override
		{ // reserved for use in future
		}

		void onDraw(sf::RenderTarget* const target) override
		{
			user.enableView(target);
			world->draw(target);
			Device::Screen::ResetView();
		}

		LocalConnection user;
		LocalWorld* const world;

	private:
		bool popingContext = false;
	};
}
