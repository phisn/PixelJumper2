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
		ManagedWorldUnlockRequest(Resource::UnlockID unlockID)
			:
			unlockID(unlockID)
		{
		}

		void onRequestFailure(const Reason reason) override
		{
			Log::Error(L"unlock request failed",
				unlockID, L"unlockID",
				(int) reason, L"reason");
		}

		void onWorldUnlocked() override
		{
			Log::Information(L"successfully unlocked",
				unlockID, L"unlockID");

			Simulator::UnlockBuffer::ReleaseUnlock(unlockID);
		}

		void onWorldUnlockFailed() override
		{
			// nothing we can do about it
			Log::Error(L"failed to unlock world");
		}

	private:
		Resource::UnlockID unlockID;
	};
}
