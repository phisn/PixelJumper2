#pragma once

#include <Client/source/game/ClassicSimulation.h>
#include <Client/source/game/ControllablePlayer.h>
#include <Client/source/net/RequestHandlerBase.h>

namespace Game::Net
{
	struct ClientClassicSimulationHandlerCallback
	{
	};

	class ClientClassicSimulationHandler
		:
		public ::Net::RequestHandler
	{
	public:
		ClientClassicSimulationHandler(
			ClientClassicSimulationHandlerCallback* const callback,
			const SimulationBootInformation info,
			const WorldResourceContainer& worldContainer,
			Device::View* const view)
			:
			callback(callback),
			info(info),
			worldContainer(worldContainer),
			player(
				Device::Input::PlayerID::P1,
				PlayerInformation{
					0,
					info.representationID,
					""
				},
				view),
			simulation(worldContainer)
		{
		}

		virtual bool initializeSimulation()
		{

		}

		void update() override
		{
		}

		bool onMessage(
			const Device::Net::MessageID messageID,
			Resource::ReadPipe* const pipe) override
		{
			switch (messageID)
			{

			}

			return false;
		}

	private:
		ClientClassicSimulationHandlerCallback* const callback;
		const SimulationBootInformation info;
		const WorldResourceContainer& worldContainer;

		ControllablePlayer player;
		ClassicSimulation simulation;
	};
}
