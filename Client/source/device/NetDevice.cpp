#include "NetDevice.h"
#include <unordered_map>

namespace
{
	std::unordered_map<HSteamListenSocket, Device::Net::Server*> servers;
	std::unordered_map<HSteamNetConnection, Device::Net::Client*> clients;

	class GNSCallbackHandler
		:
		public ISteamNetworkingSocketsCallbacks
	{
	public:
		virtual void OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* const status) override
		{
			if (status->m_info.m_hListenSocket == k_HSteamListenSocket_Invalid)
			{
				clients[status->m_hConn]->onConnectionStatusChanged(status);
			}
			else
			{
				servers[status->m_info.m_hListenSocket]->onConnectionStatusChanged(status);
			}
		}
	};
}

namespace Device::Net
{
	bool Initialize()
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

	void Uninitialize()
	{
		GameNetworkingSockets_Kill();
	}

	void Process()
	{
		static GNSCallbackHandler handler;
		static ISteamNetworkingSockets* snsInterface = SteamNetworkingSockets();

		snsInterface->RunCallbacks(&handler);
	}

	Server::Server()
	{
		networkInterface = SteamNetworkingSockets();
	}

	Server::~Server()
	{
		SteamNetConnectionInfo_t info;

		for (const HSteamNetConnection connection : connections)
			if (networkInterface->GetConnectionInfo(
					connection,
					&info) && 
				info.m_eState != k_ESteamNetworkingConnectionState_None)
			{
				networkInterface->CloseConnection(
					connection,
					2,
					"server closed",
					false);
			}

		networkInterface->CloseListenSocket(listenSocket);
		servers.erase(listenSocket);
	}

	bool Server::initialize(const sf::Uint16 port)
	{
		SteamNetworkingIPAddr localAddress;
		localAddress.Clear();
		localAddress.m_port = port;

		listenSocket = networkInterface->CreateListenSocketIP(
			localAddress,
			0,
			NULL);

		if (listenSocket == k_HSteamListenSocket_Invalid)
		{
			Log::Error(L"Failed to initialize server listener",
				(int)port, L"port");

			return false;
		}

		servers[listenSocket] = this;

		return true;
	}

	Client::Client()
	{
		connection = k_HSteamNetConnection_Invalid;
		networkInterface = SteamNetworkingSockets();
	}
	
	Client::~Client()
	{
		if (connection != k_HSteamNetConnection_Invalid)
		{
			closeConnection(4, "client destructor");
			clients.erase(connection);
		}
	}

	bool Client::connect(const SteamNetworkingIPAddr& ipAddress)
	{
		connection = networkInterface->ConnectByIPAddress(
			ipAddress,
			0,
			NULL);

		if (connection == k_HSteamNetConnection_Invalid)
		{
			Log::Error(L"Failed to create connection to server",
				ipAddress.GetIPv4(), L"ip",
				ipAddress.m_port, L"port");

			return false;
		}

		clients[connection] = this;

		return true;
	}

	void Client::closeConnection(const int reasonID, const char* reason, const bool linger)
	{
		if (!networkInterface->CloseConnection(connection, reasonID, reason, linger))
		{
			Log::Error(L"Got invalid connection while closing connection");
		}

		clients.erase(connection);
		connection = k_HSteamNetConnection_Invalid;
	}
}
