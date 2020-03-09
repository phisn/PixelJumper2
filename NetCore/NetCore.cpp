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

			networkInterface->CloseConnection(
				socket,
				0,
				"closed by peer",
				false);
			socket = k_HSteamNetConnection_Invalid;

			break;
		case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
			onConnectionLost(event->m_info.m_eEndReason);

			networkInterface->CloseConnection(
				socket,
				0,
				"problem detected locally",
				false);
			socket = k_HSteamNetConnection_Invalid;

			break;
		default:
			Log::Information(L"Got unusual new connection status",
				(int)event->m_info.m_eState, L"state");

			break;
		}
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
					Util::ConvertIPAddress(&event->m_info.m_addrRemote), L"ip");

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
