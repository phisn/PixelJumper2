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
			Log::Information(L"statuschanged",
				status->m_eOldState, L"oldstate",
				status->m_hConn, L"connection",
				status->m_info.m_eState, L"state",
				status->m_info.m_hListenSocket, L"listen");

			if (status->m_info.m_hListenSocket == k_HSteamListenSocket_Invalid)
			{
				decltype(clients)::iterator client = clients.find(status->m_hConn);

				if (client == clients.end())
				{
					Log::Warning(L"status changed for unkown client socket",
						status->m_hConn, L"socket",
						status->m_info.m_eState, L"status");
				}
				else
				{
					client->second->OnSteamNetConnectionStatusChanged(status);
				}
			}
			else
			{
				decltype(servers)::iterator server = servers.find(status->m_info.m_hListenSocket);

				if (server == servers.end())
				{
					Log::Warning(L"status changed for unkown server listen socket",
						status->m_hConn, L"socket",
						status->m_info.m_hListenSocket, L"listen",
						status->m_info.m_eState, L"status");
				}
				else
				{
					server->second->OnSteamNetConnectionStatusChanged(status);
				}
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
					Log::Independent::Information(L"NetworkUtils message (" + carrtowstr(message) + L")");
					return;
				}

				if (type >= ESteamNetworkingSocketsDebugOutputType::k_ESteamNetworkingSocketsDebugOutputType_Warning)
				{
					Log::Independent::Information(L"NetworkUtils warning (" + carrtowstr(message) + L")");
					return;
				}

				Log::Independent::Error(L"NetworkUtils different (" + carrtowstr(message) + L")");
			});

		networkInterface = SteamNetworkingSockets();

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

	const std::wstring ConvertIPAddress(SteamNetworkingIPAddr* const address)
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
		disconnect(0, "clientinterface destructor");
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

	void ClientInterface::OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* const event)
	{
		Log::Information(L"onsteamnetconnectionstatuschanged",
			(unsigned long long) event->m_hConn, L"connection",
			(unsigned long long) event->m_info.m_hListenSocket, L"listenconnection");

		if (socket != event->m_hConn &&
			socket != k_HSteamNetConnection_Invalid)
		{
			Log::Error(L"Got invalid connection in callback",
				(int)event->m_hConn, L"iconnection",
				(int)socket, L"connection",
				(int)event->m_info.m_eState, L"state");

			return;
		}

		switch (event->m_info.m_eState)
		{
		case k_ESteamNetworkingConnectionState_Connecting:
		case k_ESteamNetworkingConnectionState_None:
			// both ignored
			// connecting is only as server important

			break;
		case k_ESteamNetworkingConnectionState_Connected:
			onConnectionOpened();

			break;
		case k_ESteamNetworkingConnectionState_ClosedByPeer:
			onConnectionClosed(event->m_info.m_eEndReason);
			disconnect(0, "closed by peer");

			break;
		case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
			onConnectionLost(event->m_info.m_eEndReason);
			disconnect(0, "problem detected locally");

			break;
		default:
			Log::Information(L"Got unusual new connection status",
				(int)event->m_info.m_eState, L"state");

			break;
		}
	}

	bool ClientInterface::disconnect(const int reason, const char* const message, bool linger)
	{
		if (socket == k_HSteamNetConnection_Invalid)
		{
			return false;
		}

		const bool result = getNetworkInterface()->CloseConnection(
			socket,
			reason,
			message,
			linger);
		clients.erase(socket);
		socket = k_HSteamNetConnection_Invalid;

		return result;
	}

	ISteamNetworkingSockets* ClientInterface::getNetworkInterface() const
	{
		return networkInterface;
	}

	ServerInterface::~ServerInterface()
	{
		uninitialize();
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

	bool ServerInterface::uninitialize()
	{
		if (socket == k_HSteamNetConnection_Invalid)
		{
			return false;
		}

		const bool result = getNetworkInterface()->CloseListenSocket(socket);
		clients.erase(socket);
		socket = k_HSteamNetConnection_Invalid;

		return result;
	}

	void ServerInterface::OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* const event)
	{
		switch (event->m_info.m_eState)
		{
		case k_ESteamNetworkingConnectionState_Connected:
		case k_ESteamNetworkingConnectionState_None:
			// both ignored
			// connected is only as client important

			break;
		case k_ESteamNetworkingConnectionState_Connecting:
			if (!askClientConnect(event))
			{
				networkInterface->CloseConnection(
					socket,
					1,
					"connection was denied by parent",
					false);

				break;
			}

			if (const EResult result = networkInterface->AcceptConnection(event->m_hConn);
				result != k_EResultOK)
			{
				Log::Error(L"unable to accept conncetion",
					(int)result, L"result",
					ConvertIPAddress(&event->m_info.m_addrRemote), L"ip");

				networkInterface->CloseConnection(
					event->m_hConn,
					1,
					"closing failed to accept connection",
					false);
			}
			else
			{
				onClientConnect(event->m_hConn);
			}

			break;
		case k_ESteamNetworkingConnectionState_ClosedByPeer:
			onClientDisconnected(event->m_hConn);

			networkInterface->CloseConnection(
				event->m_hConn,
				1,
				"closing failed to accept connection",
				false);

			break;
		case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
			onClientLost(event->m_hConn);

			networkInterface->CloseConnection(
				event->m_hConn,
				1,
				"closing failed to accept connection",
				false);

			break;
		default:
			Log::Information(L"Got unusual new connection status",
				(int)event->m_info.m_eState, L"state");

			break;
		}
	}

	ISteamNetworkingSockets* ServerInterface::getNetworkInterface() const
	{
		return networkInterface;
	}
}
