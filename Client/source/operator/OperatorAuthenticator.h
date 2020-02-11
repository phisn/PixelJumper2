#pragma once

#include <Client/source/device/NetDevice.h>

#include <Operator/source/net/AuthClientMessage.h>
#include <Operator/source/net/AuthOperatorMessage.h>

namespace Operator
{
	class Authenticator
		:
		public Device::Net::Client
	{
	public:
		Authenticator()
		{
		}

		void onMessage(
			const Device::Net::MessageID messageID,
			Resource::ReadPipe* const pipe) override
		{
		}

		void onThreatIdentified(
			const Device::Net::MessageID messageID,
			const wchar_t* const note,
			const Device::Net::ThreatLevel level) override
		{
		}

	private:
		void onConnectionOpen() override
		{
		}

		void onConnectionLost(const int reason) override
		{
		}

		void onConnectionClosed(const int reason) override
		{
		}
	};
}
