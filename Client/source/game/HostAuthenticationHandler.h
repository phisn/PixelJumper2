#pragma once

#include <Client/source/device/NetDevice.h>

#include <Client/source/game/net/UserAuthClientMessage.h>
#include <Client/source/game/net/UserAuthHostMessage.h>

#include <Client/source/operator/OperatorConnectionHandler.h>

#include <Client/source/net/RequestHandlerBase.h>

namespace Game::Net
{
	struct AuthenticationHandlerCallback
	{
		virtual void onAuthenticated(const Operator::UserID userID) = 0;
		virtual void onAuthenticationDenied() = 0;
	};

	class AuthenticationHandler
		:
		public ::Net::RequestHandler
	{
	public:
		AuthenticationHandler(
			AuthenticationHandlerCallback* const callback,
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
				access->accessSendMessage(
					Host::AuthenticationMessageID::Timeout,
					NULL);

				callback->onAuthenticationDenied();
			}
		}

		bool onMessage(
			const Device::Net::MessageID messageID, 
			Resource::ReadPipe* const pipe) override
		{
			switch (messageID)
			{
			case Client::AuthenticationMessageID::Authenticate:
				if (Client::AuthenticationMessage message; loadMessage(messageID, &message, pipe))
				{
					onAuthenticate(message);
				}

				return true;
			}

			return false;
		}

	private:
		AuthenticationHandlerCallback* const callback;

		sf::Uint32 timeout;

		void onAuthenticate(const Client::AuthenticationMessage& request)
		{
			Operator::ConnectionKeySource keySource;

			memcpy(keySource.token.token,
				Operator::ConnectionHandler::GetToken().token,
				OPERATOR_HASH_SIZE);
			keySource.userID = request.userID;

			if (request.key.validate(keySource))
			{
				access->accessSendMessage(
					Host::AuthenticationMessageID::AuthenticationAccepted,
					NULL);

				callback->onAuthenticated(request.userID);
			}
			else
			{
				Host::AuthenticationRejectedMessage message;
				message.reason = Host::AuthenticationRejectedMessageContent::Reason::InvalidConnectionKey;

				access->accessSendMessage(
					Host::AuthenticationMessageID::AuthenticationRejected,
					&message);

				callback->onAuthenticationDenied();
			}
		}
	};
}
