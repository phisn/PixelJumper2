#pragma once

#include <Client/source/operator/request/OperatorRequest.h>
#include <Operator/source/net/ClassicSimulatorMessage.h>

namespace Operator
{
	class ClassicClientDataRequest
		:
		public Request
	{
	public:
		bool processEvent(
			const Device::Net::MessageID messageID,
			void* const message) override
		{
			switch (messageID)
			{
			case Operator::Net::Host::ClassicHostID::RequestClientData:
				onClientDataReceived(
					(Operator::Net::Host::RequestClientDataMessage*) message
				);

				return true;
			case Operator::Net::Host::ClassicHostID::RequestClientDataFailed:
				onClientDataFailed(
					(Operator::Net::Host::RequestClientDataFailedMessage*) message
				);

				return true;
			}

			return false;
		}

	protected:
		virtual void onClientDataReceived(Operator::Net::Host::RequestClientDataMessage* const message) = 0;
		virtual void onClientDataFailed(Operator::Net::Host::RequestClientDataFailedMessage* const message) = 0;
	};
}
