#pragma once

#include "AuthenticationRequest.h"

#include "Common/Notifier.h"

#include <functional>

namespace Operator
{
	class CommonAuthenticationRequest
		:
		public ClientAuthenticationRequest
	{
	public:
		// when authenticated with token onAuthenticated is called
		// else onAuthenticatedToken is called
		Util::Notifier<CommonAuthenticationRequest, UserID> notifyAuthenticated;
		Util::Notifier<CommonAuthenticationRequest, const char[OPERATOR_HASH_SIZE], UserID> notifyAuthenticatedToken;
		Util::Notifier<CommonAuthenticationRequest, Reason> notifyRejected;
		Util::Notifier<CommonAuthenticationRequest, ::Net::Host::RejectOperatorRegistrationMessage::Reason> notifyRegisterationRejected;
		Util::Notifier<CommonAuthenticationRequest, RequestInterface::Reason> notifyFailed;

	private:
		void onRequestFailure(const RequestInterface::Reason reason) override
		{
			ClientAuthenticationRequest::onRequestFailure(reason);
			notifyFailed.notify(reason);
		}

		void onAuthenticated(const UserID userID) override
		{
			notifyAuthenticated.notify(userID);
		}

		void onAuthenticated(
			const char token[OPERATOR_HASH_SIZE],
			const UserID userID) override
		{
			notifyAuthenticatedToken.notify(token, userID);
		}

		void onAuthenticationFailed(const Reason reason) override
		{
			notifyRejected.notify(reason);
		}

		void onRegistrationFailed(const ::Net::Host::RejectOperatorRegistrationMessage::Reason reason) override
		{
			notifyRegisterationRejected.notify(reason);
		}
	};
}
