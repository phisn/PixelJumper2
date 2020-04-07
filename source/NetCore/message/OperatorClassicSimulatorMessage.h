#pragma once

#include "OperatorCommonMessage.h"

#include "ResourceCore/ClassicPlayerResource.h"

// the client for classicsimulatior are trusted
// classic simulators and never normal users
namespace Net::Client
{
	struct OperatorClassicHostID
	{
		enum
		{
			_Begin = OperatorCommonMessageID::_Offset - 1,

			ClientData,

			UnlockRepresentation,
			UnlockWorld,
			
			_Offset
		};
	};

	struct RequestClientDataMessageContent
	{
		Operator::UserID userID;
	};

	typedef ::Net::TrivialNetworkMessage<RequestClientDataMessageContent> ClassicRequestClientDataMessage;
}

namespace Net::Host
{
	struct OperatorClassicHostID
	{
		enum
		{
			_Begin = OperatorCommonMessageID::_Offset - 1,

			ClientData,
			RequestClientDataFailed,

			_Offset
		};
	};

	struct ClassicRequestClientDataMessage
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