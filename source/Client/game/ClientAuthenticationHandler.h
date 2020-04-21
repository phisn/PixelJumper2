#pragma once

#include "GameCore/net/SimulatorAuthenticationMessage.h"
#include "NetCore/RequestHandler.h"

namespace Game
{
	enum class ClientAuthenticationFailure
	{
		InternalTimeout,
		ExternalTimeout
	};

	struct ClientAuthenticationHandlerCallback
	{
		virtual void onAuthenticated(Net::Host::AuthenticationAcceptedMessage* const answer) = 0;
	};

	struct ClientAuthenticationHandlerArguments
	{
		ClientAuthenticationHandlerCallback* callback;
		sf::Uint32 timeout;
	};

	class ClientAuthenticationHandler
		:
		public ::Net::RequestHandler
	{
	public:
		ClientAuthenticationHandler(const ClientAuthenticationHandlerArguments& arguments)
			:
			callback(arguments.callback),
			timeout(arguments.timeout)
		{
		}

		void update() override
		{
			if (--timeout == 0)
			{
				onAuthenticationFailed(ClientAuthenticationFailure::InternalTimeout);
			}
		}

		bool onMessage(
			const Net::MessageID messageID,
			Resource::ReadPipe* const pipe) override
		{
			switch (messageID)
			{
			case Net::Host::AuthenticationMessageID::AuthenticationAccepted:
			{
				Resource::ClassicPlayerResource classicResource;
				Resource::PlayerResource playerResource;

				Net::Host::AuthenticationAcceptedMessage message;

				message.resource = &playerResource;
				message.classicResource = &classicResource;

				if (loadMessage(messageID, &message, pipe))
				{
					callback->onAuthenticated(&message);
				}

				return true;
			}
			case Net::Host::AuthenticationMessageID::AuthenticationRejected:
				if (Net::Host::AuthenticationRejectedMessage message; loadMessage(messageID, &message, pipe))
				{
					onAuthenticationRejected(message.content.reason);
				}

				return true;
			case Net::Host::AuthenticationMessageID::Timeout:
				onAuthenticationFailed(ClientAuthenticationFailure::ExternalTimeout);

				return true;
			}

			return false;
		}

	protected:
		virtual void onAuthenticationRejected(const Net::Host::AuthenticationRejectedMessageContent::Reason reason) = 0;
		virtual void onAuthenticationFailed(const ClientAuthenticationFailure reason) = 0;

	private:
		ClientAuthenticationHandlerCallback* const callback;
		sf::Uint32 timeout;
	};
}
