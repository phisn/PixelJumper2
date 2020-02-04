#pragma once

#include <Client/source/device/NetDevice.h>

namespace Operator
{
	class ElementConnection
		:
		public Device::Net::ClientHandler
	{
	public:
		ElementConnection(const HSteamNetConnection connection)
			:
			ClientHandler(connection)
		{
		}

		bool process() override
		{
			ClientHandler::process();

			// work on requests
		}

	private:
		void onMessage(
			const Device::Net::MessageID messageID, 
			Resource::ReadPipe* const pipe) override
		{
			switch (messageID)
			{
			default:
				// could be a brute-force messageid finder
				this->onThreatIdentified(messageID,
					L"invalid messageid",
					Device::Net::ThreatLevel::Suspicious);

				break;
			}
		}

		void onThreatIdentified(
			const Device::Net::MessageID messageID, 
			const wchar_t* const note, 
			const Device::Net::ThreatLevel level) override
		{
			SteamNetConnectionInfo_t info;
			if (networkInterface->GetConnectionInfo(connection, &info))
			{
				Log::Warning(L"Threat identified: " + std::wstring(note),
					(int)level, L"level",
					(int)messageID,
					(int)info.m_addrRemote.GetIPv4(), L"ipv4",
					(int)info.m_addrRemote.m_port, L"port");
			}
			else
			{
				Log::Error(L"Invalid connection while threat is identified");
				Log::Warning(L"Threat identified: " + std::wstring(note),
					(int)level, L"level",
					(int)messageID, L"messageID",
					(int)connection, L"connection");
			}
		}
	};
}
