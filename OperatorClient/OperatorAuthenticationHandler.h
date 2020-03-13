#pragma once


#include "NetCore/message/OperatorAuthenticationMessage.h"
#include "NetCore/RequestHandler.h" 

namespace OperatorClient
{
	struct AuthenticationHandlerCallback
	{
		virtual void onAuthenticated() = 0;
		virtual void onAuthenticated(const Operator::AuthenticationToken& token) = 0;

		virtual void onAuthenticationFailed() = 0;
	};

	struct AuthenticationRequest
	{
		enum class Reason
		{
			Timeout,
			Rejected
		};

		virtual void onAuthenticated() = 0;
		virtual void onAuthenticationFailed(const Reason reason) = 0;
	};

	class AuthenticationHandler
		:
		public ::Net::RequestHandler
	{
	public:
		AuthenticationHandler()

		{
		}

		void update() override
		{

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
					message.authenticationToken;
				}

				return true;
			case ::Net::Host::OperatorAuthenticationMessageID::AcceptRegistration:
				if (::Net::Host::AcceptRegistrationMessage message; loadMessage(messageID, &message, pipe))
				{
					message.;
				}

				return true;
			case ::Net::Host::OperatorAuthenticationMessageID::AcceptToken:
				if (::Net::Host::AcceptTokenMessage message; loadMessage(messageID, &message, pipe))
				{
				}

				return true;
			case ::Net::Host::OperatorAuthenticationMessageID::IdleConnection:

				return true;
			case ::Net::Host::OperatorAuthenticationMessageID::RejectAuthentication:

				return true;
			case ::Net::Host::OperatorAuthenticationMessageID::RejectRegistration:

				return true;
			case ::Net::Host::OperatorAuthenticationMessageID::RejectToken:

				return true;
			case ::Net::Host::OperatorAuthenticationMessageID::Timeout:

				return true;
			}
		}


	};
}
