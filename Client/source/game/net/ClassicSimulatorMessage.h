#pragma once

#include <Client/source/game/ClassicSimulation.h>
#include <Client/source/game/net/FrameStatus.h>
#include <Client/source/game/net/ClassicSelectionMessage.h>

namespace Game::Net::Client
{
	struct ClassicSimulatorMessageID
	{
		enum
		{
			_Begin = AuthenticationMessageID::_Offset - 1,

			// request a sync instead of 
			// waiting for the next one
			RequestSync,

			// accept a server side forced
			// sync
			AcceptSync,

			PushMovement,

			_Offset
		};
	};

	class PushMovementMessage
		:
		public NetworkMessage
	{
	public:
		PackedFrameStatus* packetFrameStatus;

		bool load(Resource::ReadPipe* const pipe) override
		{
			return packetFrameStatus->readState(pipe);
		}

		bool save(Resource::WritePipe* const pipe) override
		{
			return packetFrameStatus->writeState(pipe);
		}
	};

	struct AcceptSyncMessageContent
	{
		sf::Uint64 tickCount;
	};

	typedef TrivialNetworkMessage<AcceptSyncMessageContent> AcceptSyncMessage;
}

namespace Game::Net::Host
{
	struct ClassicSimulatorMessageID
	{
		enum
		{
			_Begin = AuthenticationMessageID::_Offset - 1,

			// failed to initiated or load a world
			SimulationFailed,

			// load other player movement
			PlayerMovement,

			// used to adjust client game
			// speed acording to connection
			// slow connections should have
			// higher delays and to achive
			// that a speeddown at the client
			// is used to cause a "falling
			// behind"
			SpeedUp,
			SpeedDown,

			// measure tick delay between client
			// and server
			Ping,

			// upload world resource to client
			PushResource,

			// sync player and world properties
			PrepareSync,
			MakeSync,

			_Offset
		};
	};

	struct SimulationFailureMessageContent
	{
		ClassicSimulation::WorldFailure reason;
	};

	typedef TrivialNetworkMessage<SimulationFailureMessageContent> SimulationFailureMessage;
}
