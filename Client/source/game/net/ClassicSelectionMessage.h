#pragma once

#include <Client/source/game/net/FrameStatus.h>
#include <Client/source/game/net/SimulatorAuthenticationMessage.h>

#include <Client/source/resource/ClassicPlayerResource.h>

namespace Game::Net::Client
{
	struct ClassicSelectionMessageID
	{
		enum
		{
			_Begin = AuthenticationMessageID::_Offset - 1,

			// request simulation informations
			QuerySimulation,

			// request simulation start with
			// starting informations
			PrepareSimulation,

			_Offset
		};
	};

	struct PrepareSimulationMessageContent
	{
		Resource::WorldId world;
		Resource::RepresentationID representationID;
	};

	typedef TrivialNetworkMessage<PrepareSimulationMessageContent> PrepareSimulationMessage;
}

namespace Game::Net::Host
{
	struct ClassicSelectionMessageID
	{
		enum
		{
			_Begin = AuthenticationMessageID::_Offset - 1,

			InitializeClient,

			AcceptSimulationRequest,
			RejectSimulationRequest,

			_Offset
		};
	};

	struct InitializeClientMessage
		:
		public NetworkMessage
	{
		// them
		std::vector<Resource::PlayerResource*> players;

		// you
		std::string username;
		Resource::ClassicPlayerResource* classicResource;

		bool load(Resource::ReadPipe* const pipe) override
		{
			return pipe->readString(&username)
				&& classicResource->make(pipe);
		}
		
		bool save(Resource::WritePipe* const pipe) override
		{
			return pipe->writeString(&username)
				&& classicResource->save(pipe);
		}
	};

	struct AcceptSimulationRequestMessage
		:
		public NetworkMessage
	{
		// selected world plus surrounding
		std::vector<Resource::World*> initialWorlds;

		bool load(Resource::ReadPipe* const pipe) override
		{
			for (Resource::World* const world : initialWorlds)
				if (!world->make(pipe))
				{
					return false;
				}

			return true;
		}

		bool save(Resource::WritePipe* const pipe) override
		{
			for (Resource::World* const world : initialWorlds)
				if (!world->save(pipe))
				{
					return false;
				}

			return true;
		}
	};

	struct RejectSimulationRequestMessageContent
	{
		enum
		{
			SimulationAlreadyRunning,
			InvalidWorldID,
			InvalidRepresentationID,
			InternalResourceMissing,
			SimulationRunFailed

		} reason;
	};

	typedef TrivialNetworkMessage<RejectSimulationRequestMessageContent> RejectSimulationRequestMessage;
}
