#pragma once

#include "OperatorClient/OperatorClient.h"

#include "NetCore/message/OperatorAuthenticationMessage.h"

namespace Operator
{
	class ClientAuthenticationRequest
		:
		public RequestInterface
	{
	public:
		virtual void onRequestFailure(const RequestInterface::Reason reason) override
		{
			Operator::Client::client->onAuthenticationFailed();
		}

		enum class Reason
		{
			Timeout,
			Rejected
		};

	protected:
		virtual void onAuthenticated(const UserID userID) = 0;
		virtual void onAuthenticated(
			const char token[OPERATOR_HASH_SIZE],
			const UserID userID) = 0;

		virtual void onAuthenticationFailed(Net::Host::AuthenticationFailureReason reason) = 0;
		virtual void onAuthenticationTimeout() = 0;
		virtual void onRegistrationFailed(
			Net::Host::RejectOperatorRegistrationMessageContent::Reason reason) = 0;

		bool request_onMessage(
			const ::Net::MessageID messageID,
			Resource::ReadPipe* const pipe) override
		{
			switch (messageID)
			{
			case ::Net::Host::OperatorAuthenticationMessageID::AcceptAuthentication:
				if (::Net::Host::AcceptOperatorAuthenticationMessage message; request_loadMessage(messageID, &message, pipe))
				{
					onAuthenticated(
						message.content.authenticationToken,
						message.content.userID);
					Operator::Client::client->onAuthenticated(
						message.content.authenticationToken,
						message.content.userID);
				}

				return true;
			case ::Net::Host::OperatorAuthenticationMessageID::AcceptRegistration:
				if (::Net::Host::AcceptOperatorRegistrationMessage message; request_loadMessage(messageID, &message, pipe))
				{
					onAuthenticated(
						message.content.authenticationToken,
						message.content.userID);
					Operator::Client::client->onAuthenticated(
						message.content.authenticationToken,
						message.content.userID);
				}

				return true;
			case ::Net::Host::OperatorAuthenticationMessageID::AcceptToken:
				if (::Net::Host::AcceptOperatorTokenMessage message; request_loadMessage(messageID, &message, pipe))
				{
					onAuthenticated(message.content.userID);
					Operator::Client::client->onAuthenticated(
						NULL,
						message.content.userID);
				}

				return true;
			case ::Net::Host::OperatorAuthenticationMessageID::RejectRegistration:
				Operator::Client::client->onAuthenticationFailed();

				if (::Net::Host::RejectOperatorRegistrationMessage message; request_loadMessage(messageID, &message, pipe))
				{
					onRegistrationFailed(message.content.reason);
				}

				return true;
			case Net::Host::OperatorAuthenticationMessageID::AuthenticationFailure:
				Operator::Client::client->onAuthenticationFailed();

				if (::Net::Host::AuthenticationFailureMessage message; request_loadMessage(messageID, &message, pipe))
				{
					onAuthenticationFailed(message.content.reason);
				}

				return true;
			case ::Net::Host::OperatorAuthenticationMessageID::Timeout:
				Operator::Client::client->onAuthenticationFailed();
				onAuthenticationTimeout();

				return true;
			}

			return false;
		}
	};
}
