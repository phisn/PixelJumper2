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

		virtual void onAuthenticated(const UserID userID) = 0;
		virtual void onAuthenticated(
			const char token[OPERATOR_HASH_SIZE],
			const UserID userID) = 0;


		enum class Reason
		{
			Timeout,
			Rejected
		};

		virtual void onAuthenticationFailed(const Reason reason) = 0;
		virtual void onRegistrationFailed(
			const ::Net::Host::RejectRegistrationMessage::Reason reason) = 0;

	private:
		bool onMessage(
			const ::Net::MessageID messageID,
			Resource::ReadPipe* const pipe)
		{
			switch (messageID)
			{
			case ::Net::Host::OperatorAuthenticationMessageID::AcceptAuthentication:
				if (::Net::Host::AcceptAuthenticationMessage message; loadMessage(messageID, &message, pipe))
				{
					onAuthenticated(
						message.authenticationToken, 
						message.userID);
					Operator::Client::client->onAuthenticated(
						message.authenticationToken,
						message.userID);
				}

				return true;
			case ::Net::Host::OperatorAuthenticationMessageID::AcceptRegistration:
				if (::Net::Host::AcceptRegistrationMessage message; loadMessage(messageID, &message, pipe))
				{
					onAuthenticated(
						message.authenticationToken, 
						message.userID);
					Operator::Client::client->onAuthenticated(
						message.authenticationToken,
						message.userID);
				}

				return true;
			case ::Net::Host::OperatorAuthenticationMessageID::AcceptToken:
				if (::Net::Host::AcceptTokenMessage message; loadMessage(messageID, &message, pipe))
				{
					onAuthenticated(message.userID);
					Operator::Client::client->onAuthenticated(
						NULL,
						message.userID);
				}

				return true;
			case ::Net::Host::OperatorAuthenticationMessageID::RejectAuthentication:
				Operator::Client::client->onAuthenticationFailed();
				onAuthenticationFailed(Reason::Rejected);

				return true;
			case ::Net::Host::OperatorAuthenticationMessageID::RejectRegistration:
				Operator::Client::client->onAuthenticationFailed();

				if (::Net::Host::RejectRegistrationMessage message; loadMessage(messageID, &message, pipe))
				{
					onRegistrationFailed(message.reason);
				}

				return true;
			case ::Net::Host::OperatorAuthenticationMessageID::RejectToken:
				Operator::Client::client->onAuthenticationFailed();
				onAuthenticationFailed(Reason::Rejected);

				return true;
			case ::Net::Host::OperatorAuthenticationMessageID::Timeout:
				Operator::Client::client->onAuthenticationFailed();
				onAuthenticationFailed(Reason::Timeout);

				return true;
			}

			return false;
		}
	};
}
