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

		struct UnlockWorldMessageContent
		{
			Operator::UserID userID;
			Resource::WorldID worldID;
		};

		typedef TrivialNetworkMessage<UnlockWorldMessageContent> UnlockWorldMessage;

		struct UnlockWorldRepresentationContent
		{
			Operator::UserID userID;
			Resource::RepresentationID representationID;
		};

		typedef TrivialNetworkMessage<UnlockWorldRepresentationContent> UnlockRepresentationMessage;
	}
}

namespace Net::Host
{
	struct OperatorClassicHostID
	{
		enum
		{
			_Begin = OperatorCommonMessageID::_Offset - 1,

			UnlockedWorld,
			UnlockedRepresentation,

			// both can only fail because of
			// failed database write
			UnlockRepresentationFailed,
			UnlockWorldFailed,

			ClientRegistered,
			ClientRegistrationFailed,

			ClientUnregistered,

			ClientDataReply,
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
			Resource::ClassicPlayerResource classicResource;
			Resource::PlayerResource resource;

			bool load(Resource::ReadPipe* const pipe) override
			{
				return resource.make(pipe)
					&& classicResource.make(pipe);
			}

			bool save(Resource::WritePipe* const pipe) override
			{
				return resource.save(pipe)
					&& classicResource.save(pipe);
			}
		};

		enum class ClientDataRequestFailedReason
		{
			InvalidUserID,
			DatabaseRetriveFailed
		};

		struct ClientDataRequestFailedMessageContent
		{
			ClientDataRequestFailedReason reason;
		};

		typedef ::Net::TrivialNetworkMessage<ClientDataRequestFailedMessageContent> ClientDataRequestFailedMessage;

		typedef ClientDataMessage ClientRegisteredMessage;

		enum class ClientRegistrationFailedReason
		{
			MaxPlayerCountReached,
			UserRegisteredSomewhere,
			UserNotFound,
			DatabaseRetriveFailed
		};

		struct ClientRegistrationFailedMessageContent
		{
			ClientRegistrationFailedReason reason;
		};

		typedef TrivialNetworkMessage<ClientRegistrationFailedMessageContent> ClientRegistrationFailedMessage;
	}
}
