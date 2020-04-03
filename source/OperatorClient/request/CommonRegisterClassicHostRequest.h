#pragma once

#include "CommonRequestInterface.h"
#include "RegisterClassicHostRequest.h"

#include "Common/Notifier.h"

#include <functional>

namespace Operator
{
	class CommonRegisterClassicHostRequest
		:
		public RegisterClassicHostRequest,
		public CommonRequestInterface
	{
	public:
		// when authenticated with token onAuthenticated is called
		// else onAuthenticatedToken is called
		Util::Notifier<CommonRegisterClassicHostRequest> notifyRegisteredClassicHost;
		Util::Notifier<CommonRegisterClassicHostRequest> notifyRegisterClassicHostRejected;
		Util::Notifier<CommonRegisterClassicHostRequest, RequestInterface::Reason> notifyFailed;

	private:
		void onRequestFailure(const RequestInterface::Reason reason) override
		{
			Log::Warning(L"CommRegisterClassicHost failed", (int)reason, L"reason");

			finish(false);
			notifyFailed.notify(reason);
		}

		void onRegisteredClassicHost() override
		{
			Log::Information(L"Common registered classichost");

			finish(true);
			notifyRegisteredClassicHost.notify();
		}

		void onRegisterClassicHostRejected() override
		{
			Log::Warning(L"Common registerclassichost rejeceted");

			finish(false);
			notifyRegisterClassicHostRejected.notify();
		}
	};
}
