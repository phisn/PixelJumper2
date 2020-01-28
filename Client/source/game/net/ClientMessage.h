#pragma once

#include <Client/source/game/net/ServerMessage.h>
#include <Client/source/game/PlayerInformation.h>

#include <Client/source/resource/PlayerResource.h>

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
		public Resource::ResourceBase
	{
		Resource::PlayerResource player;

		bool make(Resource::ReadPipe* const pipe) override
		{
			return player.make(pipe);
		}

		bool save(Resource::WritePipe* const pipe) override
		{
			return player.save(pipe);
		}

		bool setup() override
		{
			return player.setup();
		}

		bool validate() override
		{
			return player.validate();
		}
	};

	struct ClientDisconnectMessage
	{
	};
}
