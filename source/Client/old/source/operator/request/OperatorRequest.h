#pragma once

#include <Client/source/device/NetDevice.h>

#include <functional>

namespace Operator
{
	struct Request
	{
		enum class Reason
		{
			AuthenticationFailed,
			ConnectionClosed,
			ConnectionLost,
			OperatorInternalError,
			OperatorTimeout,
			Timeout
		};

		virtual void onRequestFailed(const Reason reason) = 0;

		// return is finished request
		virtual bool processEvent(
			const Device::Net::MessageID messageID,
			void* const message) = 0;

		typedef std::function<void(const Reason)> RequestFailCallback;
	};
}
