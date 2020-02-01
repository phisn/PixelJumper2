#pragma once

#include <Client/source/game/PlayerInformation.h>

#include <Client/source/game/net/NetworkMessage.h>

namespace Game::Net
{
	enum class ClientMessage
	{
		Connect,
		Disconnect,
		Error,
		Framestatus
	};

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
