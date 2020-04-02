#pragma once

#include "RequestInterface.h"

#include "NetCore/message/OperatorCommonMessage.h"

namespace Operator
{
	class FindClassicHostRequest
		:
		public RequestInterface
	{
	public:
		virtual ~FindClassicHostRequest()
		{
		}

		bool onMessage(
			const ::Net::MessageID messageID,
			Resource::ReadPipe* const pipe) override
		{
			switch (messageID)
			{
			case Net::Host::OperatorCommonMessageID::HostFindClassic:
				if (Net::Host::HostFindClassicMessage message; request_loadMessage(messageID, &message, pipe))
				{
					onClassicHostFound(message);
				}

				return true;
			case Net::Host::OperatorCommonMessageID::HostFindClassicRejected:
				if (Net::Host::HostFindClassicRejectedMessage message; request_loadMessage(messageID, &message, pipe))
				{
					onClassicHostFindRejected(message);
				}

				return true;
			}

			return false;
		}

	private:
		virtual void onClassicHostFound(Net::Host::HostFindClassicMessage& message) = 0;
		virtual void onClassicHostFindRejected(Net::Host::HostFindClassicRejectedMessage& message) = 0;
	};
}