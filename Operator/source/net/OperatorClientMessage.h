#pragma once

#include <Operator/source/database/KeyTable.h>
#include <Client/source/game/net/NetworkMessage.h>

namespace Operator::Net::Client
{
	struct AuthenticationMessageID
	{
		enum
		{
			Register,
			Authenticate
		};
	};

	struct RegisterMessageContent
	{
		Database::RegistrationKey key;
	};

	typedef Game::Net::TrivialNetworkMessage<RegisterMessageContent> RegisterMessage;
}
