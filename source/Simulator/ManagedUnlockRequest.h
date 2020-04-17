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
			Log::Error(L"unlock world request failed",
				unlockID, L"unlockID",
				(int) reason, L"reason");
			delete this;
		}

		void onWorldUnlocked() override
		{
			Log::Information(L"world successfully unlocked",
				unlockID, L"unlockID");

			Simulator::UnlockBuffer::ReleaseUnlock(unlockID);
			delete this;
		}

		void onWorldUnlockFailed() override
		{
			// nothing we can do about it
			Log::Error(L"failed to unlock world");
			delete this;
		}

	private:
		Resource::UnlockID unlockID;
	};

	class ManagedRepresentationUnlockRequest
		:
		public RepresentationUnlockRequest
	{
	public:
		ManagedRepresentationUnlockRequest(Resource::UnlockID unlockID)
			:
			unlockID(unlockID)
		{
		}

		void onRequestFailure(const Reason reason) override
		{
			Log::Error(L"unlock representation request failed",
				unlockID, L"unlockID",
				(int)reason, L"reason");
			delete this;
		}

		void onRepresentationUnlocked() override
		{
			Log::Information(L"representation successfully unlocked",
				unlockID, L"unlockID");

			Simulator::UnlockBuffer::ReleaseUnlock(unlockID);
			delete this;
		}

		void onRepresentationUnlockFailed() override
		{
			// nothing we can do about it
			Log::Error(L"failed to unlock representation");
			delete this;
		}

	private:
		Resource::UnlockID unlockID;
	};
}
