#pragma once

#include "OperatorClient/request/ClassicHostRequest.h"
#include "OperatorUnlockBuffer.h"

namespace Operator
{
	// has to be created with new and not used
	// after finishing
	class ManagedWorldUnlockRequest
		:
		public WorldUnlockRequest
	{
	public:
		void onRequestFailure(const Reason reason) override
		{
			//Log::Error(L"unregister host request failed, might cause bugs",
			//	reason, L"reason");

			// deep problem
			// need to save request somewhere
			// better before request was even sent
			// and just remove the entry from file
			// when we get an answer
			// the user should never experience an
			// not unlocked world!
		}

		void onWorldUnlocked() override
		{

		}
	};
}
