#pragma once

#include <Client/source/game/CachedControllablePlayer.h>
#include <Client/source/game/ClassicSimulation.h>
#include <Client/source/game/net/ClassicSimulatorMessage.h>
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
			simulation.setPlayer(&player);
		}

		virtual bool initializeSimulation()
		{
			const ClassicSimulation::WorldFailure result = simulation.runWorld(info.worldID);

			if (result == ClassicSimulation::WorldFailure::Success)
			{
				onSimulationStarted();
				return true;
			}
			else
			{
				onSimulationFailed(result);
				return false;
			}
		}

		void processLogic()
		{
			const ClassicSimulation::WorldFailure result = simulation.processLogic();

			if (result != ClassicSimulation::WorldFailure::Success)
			{
				// call parent virtual pure

				Log::Error(L"Failed to process simulation",
					(int)result, L"result");
			}
		}

		virtual void onDraw(sf::RenderTarget* const target)
		{
			simulation.draw(target);
			player.onDraw(target);
		}

		void update() override
		{
			Client::PushMovementMessage message;
			message.packetFrameStatus = &player.packedFrameStatus;
			
			access->accessSendMessage(
				Client::ClassicSimulatorMessageID::PushMovement,
				&message);

			player.packedFrameStatus.frames.clear();
		}

		bool onMessage(
			const Device::Net::MessageID messageID,
			Resource::ReadPipe* const pipe) override
		{
			switch (messageID)
			{
			case Host::ClassicSimulatorMessageID::PlayerMovement:

				return true;
			case Host::ClassicSimulatorMessageID::PushResource:

				return true;
			}

			return false;
		}

	protected:
		virtual void onSimulationStarted() = 0;
		virtual void onSimulationFailed(const ClassicSimulation::WorldFailure reason) = 0;

	private:
		ClientClassicSimulationHandlerCallback* const callback;
		const SimulationBootInformation info;
		const WorldResourceContainer& worldContainer;

		VisualClassicSimulation simulation;
		CachedControllablePlayer player;
	};
}
