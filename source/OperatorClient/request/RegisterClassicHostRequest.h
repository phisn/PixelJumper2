#pragma once

#include "RequestInterface.h"

#include "NetCore/message/OperatorCommonMessage.h"

namespace Operator
{
	class RegisterClassicHostRequest
		:
		public RequestInterface
	{
	public:
		bool onMessage(
			const ::Net::MessageID messageID,
			Resource::ReadPipe* const pipe) override
		{
			switch (messageID)
			{
			case Net::Host::OperatorCommonMessageID::RegisterClassicHostAccepted:
				onRegisteredClassicHost();

				return true;
			case Net::Host::OperatorCommonMessageID::RegisterClassicHostRejected:
				onRegisterClassicHostRejected();

				return true;
			}

			return false;
		}

	private:
		virtual void onRegisteredClassicHost() = 0;
		virtual void onRegisterClassicHostRejected() = 0;
	};
}
