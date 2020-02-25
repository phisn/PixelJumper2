#pragma once

#include <Operator/source/Common.h>
#include <Operator/source/net/AuthenticationMessage.h>

namespace Operator::Net::Client
{
	/*
		ideas:
		-	change username
		-!	change password
	
	*/
	struct CommonRequestMessageID
	{
		enum
		{
			_Begin = AuthMessageID::_Offset - 1,

			RequestConnectionKey,

			RegisterClassicHost,

			_Offset
		};
	};

	struct RequestConnectionKeyMessageContent
	{
		UserID userID;
	};

	typedef Game::Net::TrivialNetworkMessage<RequestConnectionKeyMessageContent> RequestConnectionKeyMessage;
}

namespace Operator::Net::Host
{
	struct CommonRequestMessageID
	{
		enum
		{
			_Begin = AuthMessageID::_Offset - 1,

			InternalError,
			ConnectionKey,

			// currently is only caused by userid
			// not found
			ConnectionKeyFailed,

			RegisterClassicHostRejected,
			RegisterClassicHostAccepted,

			_Offset
		};
	};

	struct ConnectionKeyMessageContent
	{
		ConnectionKey key;
	};

	typedef Game::Net::TrivialNetworkMessage<ConnectionKeyMessageContent> ConnectionKeyMessage;
}
