#pragma once

#include "NetCore/NetCore.h"
#include "NetCore/NetworkMessage.h"
#include "ResourceCore/pipes/MemoryPipe.h"

namespace Operator
{
	class RequestInterface
	{
	public:
		virtual ~RequestInterface()
		{
		}

		enum class Reason
		{
			InternalError,
			ExternalError,

			// reason here is unkown
			// normally explained before
			ConnectionClosed,
			ConnectionLost,

			ConnectFailed,
			AuthenticateFailed,

			SendFailed,

			// disconnect was called
			// and all running requests
			// removed
			Disconnect,

			TimeoutUnconnected,
			TimeoutUnauthenticated,
			TimeoutUnresponeded,

			Load
		};

		virtual void onRequestFailure(const Reason reason) = 0;

		// true if request can be removed
		virtual bool request_onMessage(
			const ::Net::MessageID messageID,
			Resource::ReadPipe* const pipe) = 0;

	protected:
		bool request_loadMessage(
			const ::Net::MessageID messageID,
			::Net::NetworkMessage* const message,
			Resource::ReadPipe* const pipe)
		{
			if (!message->load(pipe))
			{
				Log::Warning(L"failed to load operator request",
					messageID, L"messageID");

				onRequestFailure(Reason::Load);

				return false;
			}

			return true;
		}
	};
}