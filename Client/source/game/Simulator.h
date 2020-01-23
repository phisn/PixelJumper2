#pragma once

#include <Client/source/game/GameWorld.h>
#include <Client/source/game/Simulation.h>

namespace Game
{
	class Simulator
		:
		public GameState
	{
	public:
		virtual bool initialize() = 0;
		virtual void onLogic(const sf::Time time) = 0;
	};

	class ClassicSimulator
		:
		public Simulator
	{
	public:
		ClassicSimulator()
		{
		}

		virtual void onLogic(const sf::Time time) override
		{

		}

		virtual bool pushConnection(UserConnection* const connection)
		{
			/*
			ClassicSimulation* const simulation = new ClassicSimulation(worldResources, connection);

			if (!simulation->initialize())
			{
				delete simulation;
				return false;
			}

			simulations.push_back(simulation);
			*/
		}

		virtual void removeConnection(UserConnection* const connection)
		{
		}

		bool writeState(Resource::WritePipe* const writePipe) override
		{
		}

		bool readState(Resource::ReadPipe* const readPipe) override
		{
		}

	protected:
		virtual void pushWorld(Resource::World* const world, UserConnection* const connection)
		{
		}

	private:
		virtual void processSimulations()
		{
			for (Simulation* const simulation : simulations)
				if (simulation->getStatus() == Simulation::Status::Running)
				{
					simulation->processLogic();
				}
		}

		std::vector<ClassicSimulation*> simulations;
	};

	class LocalClassicTestSimulator
		:
		public Simulator
	{
	public:
		typedef std::map<Resource::WorldId, Resource::World*> WorldResources;

		LocalClassicTestSimulator(
			LocalConnection* const connection,
			const WorldResources& worlds)
			:
			connection(connection),
			worlds(worlds)
		{
			simulation = new VisualClassicSimulation(worlds, connection);
		}

		bool initialize() override
		{
			return simulation->initialize()
				&& simulation->pushWorld(worlds.begin()->second->content.id);
		}

		void draw(sf::RenderTarget* const target)
		{
			connection->enableView(target);
			simulation->draw(target);
			connection->getLocalPlayer()->onDraw(target);
		}

		void onLogic(const sf::Time time) override
		{
			logicCounter += time.asMicroseconds();

			while (logicCounter > LogicTimeStep)
			{
				simulation->processLogic();

				logicCounter -= LogicTimeStep;
			}

			connection->getLocalPlayer()->onLogic(time);
		}

		bool writeState(Resource::WritePipe* const writePipe) override
		{
			return true;
		}

		bool readState(Resource::ReadPipe* const readPipe) override
		{
			return true;
		}

	private:
		sf::Uint64 logicCounter = 0;
		WorldResources worlds;

		VisualClassicSimulation* simulation;
		LocalConnection* const connection;
	};
}
