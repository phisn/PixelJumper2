#pragma once

#include <Client/source/game/net/ServerMessage.h>

namespace Game::Net
{
	enum class ServerMessage
	{
		Accept,
		LoadResource,
		Synchronize,

		Error,
		Disconnect
	};
}
