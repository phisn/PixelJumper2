#pragma once

#include <Client/source/game/net/NetworkMessage.h>

#include <Operator/source/Common.h>
#include <Operator/source/net/CommonRequestMessage.h>

// the client for classicsimulatior are trusted
// classic simulators and never normal users
namespace Operator::Net::Client
{
	struct ClassicHostID
	{
		enum
		{
			_Begin = CommonRequestMessageID::_Offset - 1,

			RequestClientData,

			UnlockRepresentation,
			UnlockWorld,
			
			_Offset
		};
	};

	struct RequestClientDataMessageContent
	{
		UserID userID;
	};

	typedef Game::Net::TrivialNetworkMessage<RequestClientDataMessageContent> RequestClientDataMessage;
}

namespace Operator::Net::Host
{
	struct ClassicHostID
	{
		enum
		{
			_Begin = CommonRequestMessageID::_Offset - 1,

			RequestClientData,
			RequestClientDataFailed,

			_Offset
		};
	};

	struct RequestClientDataMessageContent
	{
	};

	typedef Game::Net::TrivialNetworkMessage<RequestClientDataMessageContent> RequestClientDataMessage;

	struct RequestClientDataFailedMessageContent
	{
		enum
		{
			InvalidUserMode,
			InvalidUserID

		} type;
	};

	typedef Game::Net::TrivialNetworkMessage<RequestClientDataFailedMessageContent> RequestClientDataFailedMessage;
}
