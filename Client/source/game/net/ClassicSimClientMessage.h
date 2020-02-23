#pragma once

#include <Client/source/game/net/FrameStatus.h>

namespace Game::Net::Client
{
	struct ClassicSimMessageID
	{
		enum
		{
			_Begin = AuthenticationMessageID::_Offset - 1,

			// request simulation informations
			QuerySimulation,

			// request simulation start with
			// starting informations
			PrepareSimulation,

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

	struct PrepareSimulationMessageContent
	{
		Resource::WorldId world;
	};

	typedef TrivialNetworkMessage<PrepareSimulationMessageContent> PrepareSimulationMessage;
	
	class PushMovementMessage
		:
		public NetworkMessage
	{
	public:
		PackedFrameStatus packetFrameStatus;

		bool load(Resource::ReadPipe* const pipe) override
		{
			return frameStatus.readState(pipe);
		}

		bool save(Resource::WritePipe* const pipe) override
		{
			return frameStatus.writeState(pipe);
		}
	};

	struct AcceptSyncMessageContent
	{
		sf::Uint64 tickCount;
	};

	typedef TrivialNetworkMessage<AcceptSyncMessageContent> AcceptSyncMessage;
}
