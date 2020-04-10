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

			RegisterClient,
			UnregisterClient,
			ClientData,

			UnlockRepresentation,
			UnlockWorld,
			
			_Offset
		};
	};

	namespace OperatorClassicHost
	{
		struct RegisterClientMessageContent
		{
			Operator::UserID userID;
		};

		typedef TrivialNetworkMessage<RegisterClientMessageContent> RegisterClientMessage;

		struct UnregisterClientMessageContent
		{
			Operator::UserID userID;
		};

		typedef TrivialNetworkMessage<UnregisterClientMessageContent> UnregisterClientMessage;

		struct RequestClientDataMessageContent
		{
			Operator::UserID userID;
		};

		typedef ::Net::TrivialNetworkMessage<RequestClientDataMessageContent> RequestClientDataMessage;
	}
}

namespace Net::Host
{
	struct OperatorClassicHostID
	{
		enum
		{
			_Begin = OperatorCommonMessageID::_Offset - 1,

			ClientRegistered,
			ClientRegistrationFailed,

			ClientDataReceived,
			ClientDataRequestFailed,

			_Offset
		};
	};

	namespace OperatorClassicHost
	{
		struct ClientDataMessage
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

		struct ClientDataRequestFailedMessageContent
		{
			enum
			{
				InvalidUserMode,
				InvalidUserID

			} type;
		};

		typedef ::Net::TrivialNetworkMessage<ClientDataRequestFailedMessageContent> ClientDataRequestFailedMessage;

		struct ClientRegisteredMessage
			:
			public ::Net::NetworkMessage
		{
			ClientDataMessage message;

			bool load(Resource::ReadPipe* const pipe) override
			{
				return message.load(pipe);
			}

			bool save(Resource::WritePipe* const pipe) override
			{
				return message.save(pipe);
			}
		};

		enum class ClientRegistrationFailedReason
		{
			UserRegisteredSomewhere
		};

		struct ClientRegistrationFailedMessageContent
		{
			ClientRegistrationFailedReason reason;
		};

		typedef TrivialNetworkMessage<ClientRegistrationFailedMessageContent> ClientRegistrationFailedMessage;
	}
}
