#pragma once

#include <Client/source/device/NetDevice.h>

#include <Client/source/game/net/SimulatorAuthenticationMessage.h>
#include <Client/source/net/RequestHandlerBase.h>
#include <Client/source/operator/OperatorConnectionHandler.h>
#include <Client/source/operator/request/ClassicHostRequest.h>

namespace Game::Net
{
	struct AuthenticationHandlerCallback
	{
		virtual void onAuthenticated(
			const Operator::UserID userID,
			Operator::Net::Host::RequestClientDataMessage* const answer) = 0;
		virtual void onAuthenticationDenied() = 0;
	};

	class AuthenticationHandler
		:
		public ::Net::RequestHandler,
		public Operator::ClassicClientDataRequest
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
		Operator::UserID userID;

		void onAuthenticate(const Client::AuthenticationMessage& request)
		{
			Operator::ConnectionKeySource keySource;

			memcpy(keySource.token.token,
				Operator::ConnectionHandler::GetToken().token,
				OPERATOR_HASH_SIZE);
			keySource.userID = request.userID;

			if (request.key.validate(keySource))
			{
				Operator::Net::Client::RequestClientDataMessage message;
				message.userID = userID = request.userID;

				if (!Operator::ConnectionHandler::PushRequest(
						Operator::Net::Client::ClassicHostID::RequestClientData,
						&message,
						(Operator::ClassicClientDataRequest*) this))
				{
					sendAuthenticationRejectedMessage(
						Host::AuthenticationRejectedMessageContent::Reason::OperatorRequestFailed
					);

					Log::Error(L"Failed to retrive client data from operator",
						userID, L"userID");

					callback->onAuthenticationDenied();
				}
			}
			else
			{
				sendAuthenticationRejectedMessage(
					Host::AuthenticationRejectedMessageContent::Reason::OperatorRequestFailed
				);

				callback->onAuthenticationDenied();
			}
		}

		void sendAuthenticationRejectedMessage(const Host::AuthenticationRejectedMessageContent::Reason reason)
		{
			Host::AuthenticationRejectedMessage message;
			message.reason = reason;

			access->accessSendMessage(
				Host::AuthenticationMessageID::AuthenticationRejected,
				&message);
		}


		void onClientDataReceived(Operator::Net::Host::RequestClientDataMessage* const answer) override
		{
			Host::AuthenticationAcceptedMessage message;

			message.resource = &answer->resource;
			message.username = &answer->username;

			if (access->accessSendMessage(
					Host::AuthenticationMessageID::AuthenticationAccepted,
					&message))
			{
				callback->onAuthenticated(userID, answer);
			}
		}

		void onClientDataFailed(Operator::Net::Host::RequestClientDataFailedMessage* const message) override
		{
			Log::Error(L"Failed to retrive client data",
				(int) message->type, L"type");
			callback->onAuthenticationDenied();
		}

		void onRequestFailed(const Reason reason) override
		{
			sendAuthenticationRejectedMessage(
				Host::AuthenticationRejectedMessageContent::Reason::OperatorRequestFailed
			);

			Log::Error(L"Getclientdata request failed",
				(int)reason, L"reason");
			callback->onAuthenticationDenied();
		}
	};
}
