#pragma once

#include "CommonRequestMessage.h"

#include "Resource/ClassicPlayerResource.h"

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

	typedef ::Net::TrivialNetworkMessage<RequestClientDataMessageContent> RequestClientDataMessage;
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

	struct RequestClientDataMessage
		:
		public ::Net::NetworkMessage
	{
		Resource::ClassicPlayerResource resource;
		std::string username;

		bool load(Resource::ReadPipe* const pipe) override
		{
			return resource.make(pipe)
				&& pipe->readString(&username);
		}

		bool save(Resource::WritePipe* const pipe) override
		{
			return resource.save(pipe)
				&& pipe->writeString(&username);
		}
	};

	struct RequestClientDataFailedMessageContent
	{
		enum
		{
			InvalidUserMode,
			InvalidUserID

		} type;
	};

	typedef ::Net::TrivialNetworkMessage<RequestClientDataFailedMessageContent> RequestClientDataFailedMessage;
}
