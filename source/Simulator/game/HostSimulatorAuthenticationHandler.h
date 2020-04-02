#pragma once

#include "NetCore/NetCore.h"

#include "GameCore/net/SimulatorAuthenticationMessage.h"
#include "NetCore/RequestHandler.h"
#include "OperatorClient/OperatorClient.h"
#include "OperatorClient/request/ClassicHostRequest.h"

namespace Game
{
	struct AuthenticationHandlerCallback
	{
		virtual void onAuthenticated(
			const Operator::UserID userID,
			::Net::Host::ClassicRequestClientDataMessage* const answer) = 0;
		virtual void onAuthenticationDenied() = 0;
	};

	class AuthenticationHandler
		:
		public ::Net::RequestHandler,
		public Operator::ClassicHostClientDataRequest
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
				access->sendMessage(
					::Net::Host::AuthenticationMessageID::Timeout,
					NULL);

				callback->onAuthenticationDenied();
			}
		}

		bool onMessage(
			const ::Net::MessageID messageID,
			Resource::ReadPipe* const pipe) override
		{
			switch (messageID)
			{
			case ::Net::Client::AuthenticationMessageID::Authenticate:
				if (::Net::Client::AuthenticationMessage message; loadMessage(messageID, &message, pipe))
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

		void onAuthenticate(const ::Net::Client::AuthenticationMessage& request)
		{
			Log::Information(L"authenticate");

			Operator::ConnectionKeySource keySource;

			memcpy(keySource.token.token,
				Operator::Client::GetToken().token,
				OPERATOR_HASH_SIZE);
			keySource.userID = request.userID;

			if (request.key.validate(keySource))
			{
				::Net::Client::ClassicRequestClientDataMessage* message = new ::Net::Client::ClassicRequestClientDataMessage;
				message->userID = userID = request.userID;

				Operator::Client::PushRequestFailure result = Operator::Client::PushRequest(
					::Net::Client::OperatorClassicHostID::ClientData,
					message,
					(Operator::ClassicHostClientDataRequest*) this);

				if (result != Operator::Client::PushRequestFailure::Success)
				{
					Log::Information(L"authenticate3");
					sendAuthenticationRejectedMessage(
						::Net::Host::AuthenticationRejectedMessageContent::Reason::OperatorRequestFailed
					);

					Log::Error(L"Failed to retrive client data from operator",
						(int) result, L"reason",
						userID, L"userID");

					callback->onAuthenticationDenied();
				}

				Log::Information(L"authenticate4");
			}
			else
			{
				Log::Information(L"authentication failed");

				sendAuthenticationRejectedMessage(
					::Net::Host::AuthenticationRejectedMessageContent::Reason::OperatorRequestFailed
				);

				callback->onAuthenticationDenied();
			}
		}

		void sendAuthenticationRejectedMessage(const ::Net::Host::AuthenticationRejectedMessageContent::Reason reason)
		{
			::Net::Host::AuthenticationRejectedMessage message;
			message.reason = reason;

			access->sendMessage(
				::Net::Host::AuthenticationMessageID::AuthenticationRejected,
				&message);
		}

		void onClassicClientData(::Net::Host::ClassicRequestClientDataMessage& answer) override
		{
			::Net::Host::AuthenticationAcceptedMessage message;

			message.resource = &answer.resource;
			message.username = &answer.username;

			if (access->sendMessage(
					::Net::Host::AuthenticationMessageID::AuthenticationAccepted,
					&message))
			{
				callback->onAuthenticated(userID, &answer);
			}
			else
			{
				callback->onAuthenticationDenied();
			}
		}

		void onRequestClassicClientDataFailed(::Net::Host::HostFindClassicRejectedMessage& message) override
		{
			Log::Error(L"Failed to retrive client data",
				(int) message.type, L"type");
			callback->onAuthenticationDenied();
		}

		void onRequestFailure(const Reason reason) override
		{
			sendAuthenticationRejectedMessage(
				::Net::Host::AuthenticationRejectedMessageContent::Reason::OperatorRequestFailed
			);

			Log::Error(L"Getclientdata request failed",
				(int)reason, L"reason");
			callback->onAuthenticationDenied();
		}
	};
}
