#pragma once

#include <Client/source/game/PlayerInformation.h>

#include <Client/source/game/net/NetworkMessage.h>
#include <Client/source/operator/OperatorAccess.h>

namespace Game::Net::Client
{
	struct AuthenticationMessageContent
	{
		Resource::PlayerID playerID;
		Operator::ClientIdentifactor identificator;
	};

	typedef TrivialNetworkMessage<AuthenticationMessageContent> AuthenticationMessage;

	struct ClientConnectMessage
		:
		public NetworkMessage
	{
		Resource::PlayerResource player;

		bool load(Resource::ReadPipe* const pipe) override
		{
			return pipe->readValue(&player) && player.validate();
		}

		bool save(Resource::WritePipe* const pipe) override
		{
			return player.setup() && pipe->writeValue(&player);
		}
	};

	struct ClientDisconnectMessage
	{
	};
}
