#pragma once

#include "Common/Common.h"
#include "NetCore/NetworkMessage.h"
#include "ResourceCore/ClassicPlayerResource.h"

namespace Net::Client
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

namespace Net::Host
{
	struct AuthenticationMessageID
	{
		enum
		{
			_Begin = CommonMessageID::_Offset - 1,

			Timeout,

			AuthenticationAccepted,
			AuthenticationRejected,

			_Offset
		};
	};

	class AuthenticationAcceptedMessage
		:
		public NetworkMessage
	{
	public:
		Resource::ClassicPlayerResource* classicResource;
		Resource::PlayerResource* resource;

		bool load(Resource::ReadPipe* const pipe) override
		{
			return classicResource->make(pipe)
				&& resource->make(pipe);
		}

		bool save(Resource::WritePipe* const pipe) override
		{
			return classicResource->save(pipe)
				&& resource->save(pipe);
		}
	};

	struct AuthenticationRejectedMessageContent
	{
		enum class Reason
		{
			InvalidConnectionKey,
			UserRegisteredSomewhere,
			// we have some major problem with the operator
			// could be crashed or shutdown
			OperatorRequestFailed,
			// received authentication message
			// more than one time
			MultipleAuthentication

		} reason;
	};

	typedef TrivialNetworkMessage<AuthenticationRejectedMessageContent> AuthenticationRejectedMessage;
}

