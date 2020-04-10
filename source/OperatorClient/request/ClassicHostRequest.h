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

	class ClassicHostRegisterClientRequest
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
			case Net::Host::OperatorClassicHostID::ClientDataReceived:
				if (Net::Host::OperatorClassicHost::ClientRegisteredMessage message; request_loadMessage(messageID, &message, pipe))
				{
					onClientRegistered(message);
				}

				return true;
			case Net::Host::OperatorClassicHostID::ClientDataRequestFailed:
				if (Net::Host::OperatorClassicHost::ClientRegistrationFailedMessage message; request_loadMessage(messageID, &message, pipe))
				{
					onClientRegisterFailed(message);
				}

				return true;
			}

			return false;
		}

	protected:
		virtual void onClientRegistered(Net::Host::OperatorClassicHost::ClientRegisteredMessage &message) = 0;
		virtual void onClientRegisterFailed(Net::Host::OperatorClassicHost::ClientRegistrationFailedMessage& message) = 0;
	};
}
