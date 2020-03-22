#pragma once

#include "ClassicSelectionMessage.h"
#include "FrameStatus.h"
#include "NetPlayerMovement.h"

#include "GameCore/ClassicSimulation.h"

namespace Net::Client
{
	struct ClassicSimulatorMessageID
	{
		enum
		{
			_Begin = ClassicSelectionMessageID::_Offset - 1,

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
		Game::PackedFrameStatus* packetFrameStatus;

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

namespace Net::Host
{
	struct ClassicSimulatorMessageID
	{
		enum
		{
			_Begin = ClassicSelectionMessageID::_Offset - 1,

			// failed to initiated or load a world
			SimulationFailed,

			// load other player movement
			PushMovement,

			// add player in same location
			// with representation
			PushPlayer,

			// remove player from location
			PopPlayer,

			// used to sync with player speed
			TemporarilySpeedAdjustment,

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
		Game::ClassicSimulation::WorldFailure reason;
	};

	typedef TrivialNetworkMessage<SimulationFailureMessageContent> SimulationFailureMessage;

	class PlayerMovementMessage
		:
		public NetworkMessage
	{
	public:
		Game::NetPlayerMovement* movement;

		bool load(Resource::ReadPipe* const pipe) override
		{
			return movement->make(pipe);
		}

		bool save(Resource::WritePipe* const pipe) override
		{
			return movement->save(pipe);
		}
	};

	struct PushPlayerMessageContent
	{
		Resource::PlayerID playerID;
		Resource::RepresentationID representationID;
	};

	typedef TrivialNetworkMessage<PushPlayerMessageContent> PushPlayerMessage;

	struct PopPlayerMessageContent
	{
		Resource::PlayerID playerID;
	};

	typedef TrivialNetworkMessage<PopPlayerMessageContent> PopPlayerMessage;

	struct TemporarilySpeedAdjustmentMessageContent
	{
		float speedAdjustment;
		sf::Uint64 speedAdjustmentLength;
	};

	typedef TrivialNetworkMessage<TemporarilySpeedAdjustmentMessageContent> TemporarilySpeedAdjustmentMessage;
}
