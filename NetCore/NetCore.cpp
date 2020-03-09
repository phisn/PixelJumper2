#include "NetCore.h"

#include <unordered_map>

namespace
{
	std::unordered_map<HSteamListenSocket, Net::ServerInterface*> servers;
	std::unordered_map<HSteamNetConnection, Net::ClientInterface*> clients;

	ISteamNetworkingSockets* networkInterface;
	SteamNetworkingIdentity identity;

	class GNSCallbackHandler
		:
		public ISteamNetworkingSocketsCallbacks
	{
	public:
		void OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* const status) override
		{
			if (status->m_info.m_hListenSocket == k_HSteamListenSocket_Invalid)
			{
				clients[status->m_hConn]->OnSteamNetConnectionStatusChanged(status);
			}
			else
			{
				servers[status->m_info.m_hListenSocket]->OnSteamNetConnectionStatusChanged(status);
			}
		}
	};
}

namespace Net
{
	bool Core::Initialize()
	{
		SteamDatagramErrMsg error;
		if (!GameNetworkingSockets_Init(NULL, error))
		{
			Log::Error(L"Failed to initialize (" + carrtowstr(error) + L")");
			return false;
		}

		SteamNetworkingUtils()->SetDebugOutputFunction(
			// k_ESteamNetworkingSocketsDebugOutputType_Everything,
			k_ESteamNetworkingSocketsDebugOutputType_Msg,
			[](const ESteamNetworkingSocketsDebugOutputType type,
				const char* const message)
			{
				if (type == ESteamNetworkingSocketsDebugOutputType::k_ESteamNetworkingSocketsDebugOutputType_Msg ||
					type == ESteamNetworkingSocketsDebugOutputType::k_ESteamNetworkingSocketsDebugOutputType_Important)
				{
					Log::Information(L"NetworkUtils message (" + carrtowstr(message) + L")");
					return;
				}

				if (type >= ESteamNetworkingSocketsDebugOutputType::k_ESteamNetworkingSocketsDebugOutputType_Warning)
				{
					Log::Information(L"NetworkUtils warning (" + carrtowstr(message) + L")");
					return;
				}

				Log::Error(L"NetworkUtils different (" + carrtowstr(message) + L")",
					(int)type, L"type");
			}
		);

		return true;
	}

	void Core::Uninitialize()
	{
		GameNetworkingSockets_Kill();
	}

	const SteamNetworkingIdentity* GetIdentity()
	{
		return &identity;
	}

	void Core::Process()
	{
		static GNSCallbackHandler handler;
		networkInterface->RunCallbacks(&handler);
	}

	const std::wstring Util::ConvertIPAddress(SteamNetworkingIPAddr* const address)
	{
		// gurrantead to have leading zero byte
		char buffer[SteamNetworkingIPAddr::k_cchMaxString] = { };

		address->ToString(
			buffer,
			SteamNetworkingIPAddr::k_cchMaxString,
			true);

		return carrtowstr(buffer);
	}

	ClientInterface::~ClientInterface()
	{
		clients.erase(socket);
		
		if (socket != k_HSteamNetConnection_Invalid)
			networkInterface->CloseConnection(
				socket, 
				0, 
				"clientinterface destructor",
				false);
	}

	bool ClientInterface::connect(const SteamNetworkingIPAddr& address)
	{
		socket = networkInterface->ConnectByIPAddress(
			address,
			0,
			NULL);


		if (socket == k_HSteamNetConnection_Invalid)
		{
			Log::Error(L"Failed to create connection to server",
				address.GetIPv4(), L"ip",
				address.m_port, L"port");

			return false;
		}

		clients[socket] = this;
		return true;
	}

	ISteamNetworkingSockets* ClientInterface::getNetworkInterface() const
	{
		return networkInterface;
	}

	ServerInterface::~ServerInterface()
	{
		servers.erase(socket);

		if (socket != k_HSteamListenSocket_Invalid)
			networkInterface->CloseListenSocket(socket);
	}

	bool ServerInterface::initialize(const sf::Uint16 port)
	{
		SteamNetworkingIPAddr address;
		address.Clear();
		address.m_port = port;

		socket = networkInterface->CreateListenSocketIP(
			address,
			0,
			NULL);

		if (socket == k_HSteamListenSocket_Invalid)
		{
			Log::Error(L"Failed to initialize server listener",
				(int)port, L"port");

			return false;
		}

		servers[socket] = this;
		return true;
	}

	ISteamNetworkingSockets* ServerInterface::getNetworkInterface() const
	{
		return networkInterface;
	}
}
