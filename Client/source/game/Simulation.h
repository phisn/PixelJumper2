#pragma once

#include <Client/source/game/GamemodeBase.h>
#include <Client/source/game/GameWorld.h>

namespace Game
{
	class OfflineLocalSimulation
		:
		public Simulation
	{
	public:
		OfflineLocalSimulation()
		{
		}

		void onConnectionRemoved(UserConnection* const connection)
		{
		}

	private:
		bool processLogic()
		{
			gamemode->processLogic();
		}
	};

	class DedicatedLocalSimulation
		:
		public Simulation
	{
	public:
		DedicatedLocalSimulation(LocalConnection* const connection)
			:
			connection(connection)
		{
		}

		void removeConnection(UserConnection* const connection) override
		{
			if (connection == this->connection)
				kill();
		}

	private:
		bool processLogic() override
		{
			gamemode->processLogic();
		}

		LocalConnection* const connection;
	};

	class Simulator
		:
		GameState
	{
	public:
		virtual bool initialize() = 0;

		virtual void onLogic(const sf::Time time) = 0;

		virtual void removeConnection(UserConnection* const connection) = 0;
		virtual bool pushConnection(UserConnection* const connection) = 0;

	protected:
		virtual void onSimulationDead(Simulation* const simulation) = 0;
		virtual void onSimulationError(Simulation* const simulation) = 0;
	};

	class SharedSimulator
		:
		public Simulator
	{
	public:
		typedef std::vector<Simulation*> SimulationContainer;
		typedef SimulationContainer::iterator SimulationIterator;

		SharedSimulator(GamemodeCreatorBase* const gamemodeCreator)
			:
			gamemodeCreator(gamemodeCreator)
		{
		}

		virtual void onLogic(const sf::Time time)
		{
			for (Simulation* const simulation : simulations)
			{
				processSimulationLogic(simulation, time);

				switch (simulation->getStatus())
				{
				case Simulation::Status::Dead:
					onSimulationDead(simulation);

					break;
				case Simulation::Status::Error:
					onSimulationError(simulation);

					break;
				}
			}
		}

		virtual void removeConnection(UserConnection* const connection)
		{
			for (Simulation* const simulation : simulations)
			{
				simulation->removeConnection(connection);
			}
		}

	protected:
		virtual void processSimulationLogic(
			Simulation* const simulation,
			const sf::Time time)
		{
			simulation->onLogic(time);
		}

		virtual void onSimulationDead(Simulation* const simulation) override
		{
			handleCommonSimulationFault(simulation);
		}

		virtual void onSimulationError(Simulation* const simulation) override
		{
			handleCommonSimulationFault(simulation);
		}

		virtual void handleCommonSimulationFault(Simulation* const simulation)
		{
			for (SimulationIterator iterator = simulations.begin(); iterator != simulations.end(); ++iterator)
				if (*iterator != simulation)
				{
					simulations.erase(iterator);
					break;
				}

			delete simulation;
		}

		virtual bool pushSimulation(Simulation* const simulation)
		{
			if (simulation->initialize(gamemodeCreator->createGamemode()))
			{
				simulations.push_back(simulation);
				return true;
			}
			else
			{
				// TODO ?
				delete simulation;
				return false;
			}
		}

	private:
		SimulationContainer simulations;
		GamemodeCreatorBase* const gamemodeCreator;
	};

	class DedicatedSimulator
		:
		public Simulator
	{
	public:
		DedicatedSimulator(GamemodeCreatorBase* const gamemodeCreator)
			:
			gamemodeCreator(gamemodeCreator)
		{
		}

		void onLogic(const sf::Time time) override
		{
			if (simulation)
			{
				if (simulation->getStatus() != Simulation::Status::Running)
				{

				}

				simulation->onLogic(time);
			}
		}

		virtual void removeConnection(UserConnection* const connection) override
		{
			simulation->removeConnection(connection);
		}

		bool writeState(Resource::WritePipe* const writePipe) override
		{
			return simulation->writeState(writePipe);
		}

		bool readState(Resource::ReadPipe* const readPipe) override
		{
			return simulation->readState(readPipe);
		}

	protected:
		void onSimulationDead(Simulation* const simulation) override
		{
			handleCommonSimulationFault(simulation);
		}

		void onSimulationError(Simulation* const simulation) override
		{
			handleCommonSimulationFault(simulation);
		}

		virtual void handleCommonSimulationFault(Simulation* const simulation)
		{
			delete simulation;
		}

		virtual bool pushSimulation(Simulation* const simulation)
		{
			if (!simulation->initialize(gamemodeCreator->createGamemode()))
			{
				return false;
			}

			if (this->simulation == NULL)
				delete this->simulation;

			this->simulation = simulation;

			return true;
		}

	private:
		Simulation* simulation = NULL;
		GamemodeCreatorBase* const gamemodeCreator;
	};

	class DedicatedTestSimulator
		:
		public DedicatedSimulator
	{
	public:
		/*
			Connection
		

		*/

		DedicatedTestSimulator(Resource::World* const world)
			:
			DedicatedSimulator(
				new TestGamemodeCreator(
					world,
					connection
				)
			),
			connection(connection)
		{
		}

		bool pushConnection(UserConnection* const connection) override
		{

		}

		bool initialize()
		{
			return pushSimulation(new DedicatedLocalSimulation(connection));
		}
		
	private:
		LocalConnection* const connection;
	};
}
