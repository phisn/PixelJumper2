#pragma once

#include <Client/source/game/net/NetworkMessage.h>

#include <Operator/source/Common.h>
#include <Operator/source/net/AuthOperatorMessage.h>

namespace Operator::Net::Host
{
	struct RequestMessageID
	{
		enum
		{
			_Begin = AuthMessageID::_Offset - 1,

			InternalError,

			ConnectionKey,

			// currently is only caused by userid
			// not found
			ConnectionKeyFailed,

			_Offset
		};
	};

	struct ConnectionKeyMessageContent
	{
		ConnectionKey key;
	};

	typedef Game::Net::TrivialNetworkMessage<ConnectionKeyMessageContent> ConnectionKeyMessage;
}
