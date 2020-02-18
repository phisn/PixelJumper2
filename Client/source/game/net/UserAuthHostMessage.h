#pragma once

#include <Client/source/game/net/NetworkMessage.h>

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
