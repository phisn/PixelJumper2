#pragma once

#include "request/AuthenticationRequest.h"

#include "NetCore/RequestHandler.h" 

namespace Operator
{
	struct AuthenticationHandlerCallback
	{
		enum class Reason
		{
			Timeout,
			Rejected
		};

		virtual void onAuthenticated(const UserID userID) = 0;
		virtual void onAuthenticated(
			const char token[OPERATOR_HASH_SIZE],
			const UserID userID) = 0;

		virtual void onAuthenticationFailed(const Reason reason) = 0;

		virtual void onRegistrationFailed(
			const ::Net::Host::RejectRegistrationMessage::Reason reason) = 0;
	};

	class AuthenticationHandler
		:
		public ::Net::RequestHandler
	{
	public:
		AuthenticationHandler(AuthenticationHandlerCallback* const callback)
			:
			callback(callback)
		{
		}

		void update() override
		{
			if (isAuthenticating())
			{
				counter += interval;
				
				if (counter > timeout)
				{
					request->onRequestFailure(RequestInterface::Reason::Internal);

					delete message;

					request = NULL;
					message = NULL;
				}
			}
		}
		
		bool onMessage(
			const ::Net::MessageID messageID,
			Resource::ReadPipe* const pipe) override
		{
			switch (messageID)
			{
			case ::Net::Host::OperatorAuthenticationMessageID::AcceptAuthentication:
				if (::Net::Host::AcceptAuthenticationMessage message; loadMessage(messageID, &message, pipe))
				{
					callback->onAuthenticated(message.authenticationToken, message.userID);
				}

				return true;
			case ::Net::Host::OperatorAuthenticationMessageID::AcceptRegistration:
				if (::Net::Host::AcceptRegistrationMessage message; loadMessage(messageID, &message, pipe))
				{
					callback->onAuthenticated(message.authenticationToken, message.userID);
				}

				return true;
			case ::Net::Host::OperatorAuthenticationMessageID::AcceptToken:
				if (::Net::Host::AcceptTokenMessage message; loadMessage(messageID, &message, pipe))
				{
					callback->onAuthenticated(message.userID);
				}

				return true;
			case ::Net::Host::OperatorAuthenticationMessageID::RejectAuthentication:
				callback->onAuthenticationFailed(AuthenticationHandlerCallback::Reason::Rejected);
				
				return true;
			case ::Net::Host::OperatorAuthenticationMessageID::RejectRegistration:
				if (::Net::Host::RejectRegistrationMessage message; loadMessage(messageID, &message, pipe))
				{
					callback->onRegistrationFailed(message.reason);
				}

				return true;
			case ::Net::Host::OperatorAuthenticationMessageID::RejectToken:
				callback->onAuthenticationFailed(AuthenticationHandlerCallback::Reason::Rejected);

				return true;
			case ::Net::Host::OperatorAuthenticationMessageID::Timeout:
				callback->onAuthenticationFailed(AuthenticationHandlerCallback::Reason::Timeout);

				return true;
			}

			return false;
		}

		virtual bool pushRequest(
			const ::Net::MessageID messageID,
			::Net::NetworkMessage* const message,
			ClientAuthenticationRequest* const request,
			bool sendMessage)
		{
			if (isAuthenticating())
			{
				request->onRequestFailure(RequestInterface::Reason::Internal);

				return false;
			}

			this->message = message;
			this->request = request;

			counter = sf::Time::Zero;

			return true;
		}

		bool isAuthenticating() const 
		{
			return message == NULL || request == NULL;
		}

	private:
		AuthenticationHandlerCallback* const callback;

		::Net::NetworkMessage* message = NULL;
		ClientAuthenticationRequest* request = NULL;

		sf::Time interval, counter, timeout;
	};
}
