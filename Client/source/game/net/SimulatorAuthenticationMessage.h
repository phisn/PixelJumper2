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

namespace Game::Net::Host
{
	struct AuthenticationMessageID
	{
		enum
		{
			_Begin = CommonMessageID::_Offset - 1,

			InternalError,
			Timeout,

			AuthenticationAccepted,
			AuthenticationRejected,

			_Offset
		};
	};

	struct AuthenticationRejectedMessageContent
	{
		enum class Reason
		{
			InvalidConnectionKey,
			UserIDUsed

		} reason;
	};

	typedef TrivialNetworkMessage<AuthenticationRejectedMessageContent> AuthenticationRejectedMessage;
}

