#pragma once

#include <Client/source/game/net/SimulatorAuthenticationMessage.h>
#include <Client/source/net/RequestHandlerBase.h>

namespace Game::Net
{
	enum class ClientAuthenticationFailure
	{
		InternalTimeout,
		ExternalTimeout
	};

	struct ClientAuthenticationHandlerCallback
	{
		virtual void onAuthenticated(Host::AuthenticationAcceptedMessage* const answer) = 0;
	};

	class ClientAuthenticationHandler
		:
		public ::Net::RequestHandler
	{
	public:
		ClientAuthenticationHandler(
			ClientAuthenticationHandlerCallback* const callback,
			const sf::Uint32 timeout)
			:
			callback(callback),
			timeout(timeout)
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
			const Device::Net::MessageID messageID,
			Resource::ReadPipe* const pipe) override
		{
			switch (messageID)
			{
			case Host::AuthenticationMessageID::AuthenticationAccepted:
			{
				Resource::ClassicPlayerResource playerResource;
				std::string username;

				Host::AuthenticationAcceptedMessage message;

				message.resource = &playerResource;
				message.username = &username;

				if (loadMessage(messageID, &message, pipe))
				{
					callback->onAuthenticated(&message);
				}

				return true;
			}
			case Host::AuthenticationMessageID::AuthenticationRejected:
				if (Host::AuthenticationRejectedMessage message; loadMessage(messageID, &message, pipe))
				{
					onAuthenticationRejected(message.reason);
				}

				return true;
			case Host::AuthenticationMessageID::Timeout:
				onAuthenticationFailed(ClientAuthenticationFailure::ExternalTimeout);

				return true;
			}

			return false;
		}

	protected:
		virtual void onAuthenticationRejected(const Host::AuthenticationRejectedMessage::Reason reason) = 0;
		virtual void onAuthenticationFailed(const ClientAuthenticationFailure reason) = 0;

	private:
		ClientAuthenticationHandlerCallback* const callback;
		sf::Uint32 timeout;
	};
}
