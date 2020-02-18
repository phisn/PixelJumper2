#pragma once

#include <Client/source/game/net/NetworkMessage.h>

#include <Operator/source/Common.h>
#include <Operator/source/net/AuthClientMessage.h>

namespace Operator::Net::Client
{
	struct RequestMessageID
	{
		enum
		{
			_Begin = AuthMessageID::_Offset - 1,

			RequestConnectionKey,
			
			_Offset
		};
	};

	struct RequestConnectionKeyMessageContent
	{
		UserID userID;
	};
	
	typedef Game::Net::TrivialNetworkMessage<RequestConnectionKeyMessageContent> RequestConnectionKeyMessage;
}
