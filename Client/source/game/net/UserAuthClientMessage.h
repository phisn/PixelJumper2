#pragma once

#include <Client/source/game/net/NetworkMessage.h>
#include <Operator/source/Common.h>

namespace Game::Net::Client
{
	struct AuthenticationMessageID
	{
		enum
		{
			_Begin = CommonMessageID::_Offset - 1,

			Authenticate,

			_Offset
		};
	};

	struct AuthenticationMessageContent
	{
		Operator::ConnectionKey key;
		Operator::UserID userID;
	};

	typedef TrivialNetworkMessage<AuthenticationMessageContent> AuthenticationMessage;
}
