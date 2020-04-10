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
			Operator::UserID userID,
			std::string& username,
			Resource::ClassicPlayerResource& classicResource) = 0;
		virtual void onAuthenticationDenied() = 0;
	};

	class AuthenticationHandler
		:
		public ::Net::RequestHandler,
		public Operator::ClassicHostRegisterClientRequest
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

		// prevent repetetive sending of authentication messages
		bool awaitingOperatorAnswer = false;

		void onAuthenticate(const ::Net::Client::AuthenticationMessage& request)
		{
			if (awaitingOperatorAnswer)
			{
				sendAuthenticationRejectedMessage(
					::Net::Host::AuthenticationRejectedMessageContent::Reason::MultipleAuthentication
				);

				Log::Error(L"received authentication message multiple times",
					userID, L"userID");

				access->onThreatIdentified(
					::Net::Client::AuthenticationMessageID::Authenticate,
					L"client tried to authenticate multiple times",
					Net::ThreatLevel::Suspicious);

				callback->onAuthenticationDenied();
			}

			Operator::ConnectionKeySource keySource;

			memcpy(keySource.token.token,
				Operator::Client::GetToken().token,
				OPERATOR_HASH_SIZE);
			keySource.userID = request.userID;

			if (request.key.validate(keySource))
			{
				awaitingOperatorAnswer = true;

				::Net::Client::OperatorClassicHost::RegisterClientMessage* message = new ::Net::Client::OperatorClassicHost::RegisterClientMessage;
				message->userID = userID = request.userID;

				Operator::Client::PushRequestFailure result = Operator::Client::PushRequest(
					::Net::Client::OperatorClassicHostID::RegisterClient,
					message,
					(Operator::ClassicHostRegisterClientRequest*) this);

				if (result != Operator::Client::PushRequestFailure::Success)
				{
					sendAuthenticationRejectedMessage(
						::Net::Host::AuthenticationRejectedMessageContent::Reason::OperatorRequestFailed
					);

					Log::Error(L"Failed to retrive client data from operator",
						(int) result, L"reason",
						userID, L"userID");

					callback->onAuthenticationDenied();
				}
				else
				{
					Log::Information(L"authentication succeeded, awaiting operator confirmation",
						userID, L"userID");
				}
			}
			else
			{
				Log::Information(L"authentication failed",
					userID, L"userID");

				sendAuthenticationRejectedMessage(
					::Net::Host::AuthenticationRejectedMessageContent::Reason::InvalidConnectionKey
				);

				access->onThreatIdentified(
					::Net::Client::AuthenticationMessageID::Authenticate,
					L"got invalid authentication key",
					Net::ThreatLevel::Suspicious);

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

		void onClientRegistered(Net::Host::OperatorClassicHost::ClientRegisteredMessage& answer) override
		{
			Log::Information(L"received client data for",
				answer.message.username, L"username",
				userID, L"userID");

			::Net::Host::AuthenticationAcceptedMessage message;

			message.resource = &answer.message.resource;
			message.username = &answer.message.username;

			if (access->sendMessage(
					::Net::Host::AuthenticationMessageID::AuthenticationAccepted,
					&message))
			{
				callback->onAuthenticated(
					userID, 
					answer.message.username,
					answer.message.resource);
			}
			else
			{
				callback->onAuthenticationDenied();
			}
		}

		void onClientRegisterFailed(Net::Host::OperatorClassicHost::ClientRegistrationFailedMessage& message) override
		{
			Log::Error(L"Failed to retrive client data",
				(int) message.reason, L"type");
			
			switch (message.reason)
			{
			case Net::Host::OperatorClassicHost::ClientRegistrationFailedReason::UserRegisteredSomewhere:
				sendAuthenticationRejectedMessage(
					::Net::Host::AuthenticationRejectedMessageContent::Reason::UserRegisteredSomewhere
				);

				break;
			default:
				Log::Error(L"missing reason case in hostsimulatorauthenticationhandler in onclientregisterfailed",
					message.reason, L"reason");

				sendAuthenticationRejectedMessage(
					::Net::Host::AuthenticationRejectedMessageContent::Reason::OperatorRequestFailed
				);

				break;
			}

			callback->onAuthenticationDenied();
		}

		void onRequestFailure(const Reason reason) override
		{
			sendAuthenticationRejectedMessage(
				::Net::Host::AuthenticationRejectedMessageContent::Reason::OperatorRequestFailed
			);

			Log::Error(L"Getclientdata request failed",
				(int) reason, L"reason");
			callback->onAuthenticationDenied();
		}
	};
}
