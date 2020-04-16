#pragma once

#include "OperatorClient/request/ClassicHostRequest.h"

namespace Operator
{
	// has to be created with new and not used
	// after finishing
	class ManagedUnregisterClientRequest
		:
		public ClassicHostUnregisterClientRequest
	{
	public:
		void onRequestFailure(const Reason reason) override
		{
			Log::Error(L"unregister host request failed, might cause bugs",
				reason, L"reason");
		}

		void onClientUnregistered() override
		{
			delete this;
		}
	};
}
