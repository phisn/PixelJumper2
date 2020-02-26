#pragma once

#include <Client/source/operator/OperatorRequest.h>
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

				return true;

			case Operator::Net::Host::ClassicHostID::RequestClientDataFailed:


				return true;
			}

			return false;
		}

	protected:
		virtual void onClientDataReceived(Operator::) = 0;
		virtual void onClientDataFailed() = 0;
	};
}
