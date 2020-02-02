#pragma once

#include <Client/source/game/PlayerInformation.h>

#include <Client/source/game/net/NetworkMessage.h>
#include <Client/source/operator/OperatorAccess.h>

namespace Game::Net::Client
{
	struct AuthenticationMessageID
	{
		enum
		{
			_Begin = CommonMessageID::_Offset - 1,

			// request auth from the host
			Authenticate,

			_Offset
		};
	};

	struct AuthenticationMessageContent
	{
		Resource::PlayerID playerID;
		Operator::ClientIdentifactor identificator;
	};

	typedef TrivialNetworkMessage<AuthenticationMessageContent> AuthenticationMessage;

	struct ClassicalConnectionMessageID
	{
		enum
		{
			_Begin = AuthenticationMessageID::_Offset - 1,

			// request simulation informations
			QuerySimulation,

			// request simulation start with
			// starting informations
			RequestSimulation,

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

	struct QuarySimulationMessage
	{
	};

	struct OpenSimulationMessageContent
	{
		Resource::WorldId world;
	};

	typedef TrivialNetworkMessage<OpenSimulationMessageContent> RequestSimulationMessage;

	struct RequestSyncMessage
	{
	};

	struct AcceptSyncMessageContent
	{
		sf::Uint64 tickCount;
	};

	typedef TrivialNetworkMessage<AcceptSyncMessageContent> AcceptSyncMessage;
}
