#pragma once

#include "RequestInterface.h"

#include "NetCore/message/OperatorClassicSimulatorMessage.h"

namespace Operator
{
	class ClassicHostClientDataRequest
		:
		public RequestInterface
	{
	public:
		bool request_onMessage(
			const ::Net::MessageID messageID,
			Resource::ReadPipe* const pipe) override
		{
			switch (messageID)
			{
			case Net::Host::OperatorClassicHostID::ClientData:
				if (Net::Host::ClassicRequestClientDataMessage message; request_loadMessage(messageID, &message, pipe))
				{
					onClassicClientData(message);
				}

				return true;
			case Net::Host::OperatorClassicHostID::RequestClientDataFailed:
				if (Net::Host::HostFindClassicRejectedMessage message; request_loadMessage(messageID, &message, pipe))
				{
					onRequestClassicClientDataFailed(message);
				}

				return true;
			}

			return false;
		}

	protected:
		virtual void onClassicClientData(Net::Host::ClassicRequestClientDataMessage& message) = 0;
		virtual void onRequestClassicClientDataFailed(Net::Host::HostFindClassicRejectedMessage& message) = 0;
	};
}
