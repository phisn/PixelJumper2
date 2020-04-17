#pragma once

#include "AuthenticationRequest.h"
#include "CommonRequestInterface.h"

#include "Common/Notifier.h"

#include <functional>

namespace Operator
{
	class CommonAuthenticationRequest
		:
		public ClientAuthenticationRequest,
		public CommonRequestInterface
	{
	public:
		// when authenticated with token onAuthenticated is called
		// else onAuthenticatedToken is called
		Util::Notifier<CommonAuthenticationRequest, UserID> notifyAuthenticated;
		Util::Notifier<CommonAuthenticationRequest, const char[OPERATOR_HASH_SIZE], UserID> notifyAuthenticatedToken;
		Util::Notifier<CommonAuthenticationRequest> notifyTimeout;
		Util::Notifier<CommonAuthenticationRequest, Net::Host::AuthenticationFailureReason> notifyRejected;
		Util::Notifier<CommonAuthenticationRequest, ::Net::Host::RejectOperatorRegistrationMessageContent::Reason> notifyRegisterationRejected;
		Util::Notifier<CommonAuthenticationRequest, RequestInterface::Reason> notifyFailed;

	private:
		void onAuthenticationTimeout() override
		{
			Log::Warning(L"CommonAuthenticationRequest timed out");
			notifyTimeout.notify();
		}

		void onRequestFailure(const RequestInterface::Reason reason) override
		{
			Log::Warning(L"CommonAuthenticationRequest failed", (int) reason, L"reason");

			finish(false);
			ClientAuthenticationRequest::onRequestFailure(reason);
			notifyFailed.notify(reason);
		}

		void onAuthenticated(const UserID userID) override
		{
			Log::Information(L"CommonAuthenticationRequest succeeded", userID, L"userID");

			finish(true);
			notifyAuthenticated.notify(userID);
		}

		void onAuthenticated(
			const char token[OPERATOR_HASH_SIZE],
			const UserID userID) override
		{
			Log::Information(L"CommonAuthenticationRequest succeeded (wtoken)", userID, L"userID");

			finish(true);
			notifyAuthenticatedToken.notify(token, userID);
		}

		void onAuthenticationFailed(Net::Host::AuthenticationFailureReason reason) override
		{
			Log::Warning(L"Common authentication failed", (int) reason, L"reason");

			finish(false);
			notifyRejected.notify(reason);
		}

		void onRegistrationFailed(const ::Net::Host::RejectOperatorRegistrationMessageContent::Reason reason) override
		{
			Log::Warning(L"Common registration failed", (int)reason, L"reason");

			finish(false);
			notifyRegisterationRejected.notify(reason);
		}
	};
}
