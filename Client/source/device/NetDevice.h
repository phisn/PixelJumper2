#pragma once

#include <Client/source/device/RandomDevice.h>
#include <Client/source/logger/Logger.h>
#include <Client/source/resource/pipes/MemoryPipe.h>

#include <steam/steamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>

#define DEV_NET_PORT 9928

namespace Device::Net
{
	bool Initialize();
	void Uninitialize();

	class ClientHandler
	{
	protected:
		ClientHandler()
		{
		}

	public:
		ClientHandler(const HSteamNetConnection connection)
			:
			connection(connection)
		{
			networkInterface = SteamNetworkingSockets();
		}

		virtual ~ClientHandler()
		{
		}

		virtual bool process()
		{
			while (true)
			{
				ISteamNetworkingMessage* message = NULL;
				
				const int count = networkInterface->ReceiveMessagesOnConnection(
					connection,
					&message,
					4);

				if (count == 0)
				{
					return true;
				}

				if (count < 0)
				{
					Log::Error(L"Got invalid connection in clienthandler receive");
					return false;
				}

				for (int i = 0; i < count; ++i)
				{
					Resource::MemoryReadPipe pipe(
						(const char*) message->m_pData,
						message->m_cbSize);

					onMessage(&pipe);

					message->Release();
					++message;
				}
			}
		}

	protected:
		virtual void onMessage(Resource::ReadPipe* const pipe) = 0;

		Resource::WritePipe* beginMessage(const int reserve = 32)
		{
			messagePipe.reset();
			messagePipe.reserveSize(reserve);

			return &messagePipe;
		}

		void sendMessage()
		{
			const EResult result = networkInterface->SendMessageToConnection(
				connection,
				&messagePipe.getData()[0],
				messagePipe.getSize(),
				k_nSteamNetworkingSend_Reliable,
				NULL);

			if (result != EResult::k_EResultOK)
			{
				Log::Error(L"Failed to send message",
					(int) result, L"result",
					messagePipe.getSize(), L"size");
			}
		}

		ISteamNetworkingSockets* networkInterface;
		HSteamNetConnection connection;

	private:
		Resource::MemoryWritePipe messagePipe;
	};

	class Server
		:
		public ISteamNetworkingSocketsCallbacks
	{
	public:
		virtual ~Server()
		{
			SteamNetConnectionInfo_t info;

			for (const HSteamNetConnection connection : connections)
			{
				if (networkInterface->GetConnectionInfo(
						connection,
						&info))
				{
					if (info.m_eState != k_ESteamNetworkingConnectionState_None)
					{
						networkInterface->CloseConnection(
							connection,
							2,
							"server closed",
							false);
					}
				}
			}

			networkInterface->CloseListenSocket(listenSocket);
		}

		virtual bool Initialize(const sf::Uint16 port = DEV_NET_PORT)
		{
			networkInterface = SteamNetworkingSockets();
			
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
					(int) port, L"port");

				return false;
			}

			return true;
		}

		void process()
		{
			networkInterface->RunCallbacks(this);
		}

	protected:
		ISteamNetworkingSockets* networkInterface;
		std::vector<HSteamNetConnection> connections;

		bool existsConnection(const HSteamNetConnection connection)
		{
			for (HSteamNetConnection i : connections)
				if (connection == i)
				{
					return true;
				}

			return false;
		}

		bool removeConnection(const HSteamNetConnection connection)
		{
			decltype(connections)::const_iterator iterator = std::find(
				connections.cbegin(),
				connections.cend(),
				connection);

			if (iterator == connections.cend())
			{
				return false;
			}
			else
			{
				connections.erase(iterator);
				return true;
			}
		}

		// ask weather accept the connection or
		// deny. allows denial based on ipaddress
		virtual bool askClientConnect(SteamNetworkingIPAddr* const ipAddress) = 0;
		virtual void onClientConnect(const HSteamNetConnection connection) = 0;

		virtual void onClientDisconnected(const HSteamNetConnection connection) = 0;
		virtual void onClientLost(const HSteamNetConnection connection) = 0;

	private:
		void OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* const event) override
		{
			switch (event->m_info.m_eState)
			{
			case k_ESteamNetworkingConnectionState_Connected:
			case k_ESteamNetworkingConnectionState_None:
				// both ignored
				// connected is only as client important

				break;
			case k_ESteamNetworkingConnectionState_Connecting:
				if (existsConnection(event->m_hConn))
				{
					Log::Error(L"Connection already exists",
						event->m_info.m_addrRemote.GetIPv4(), L"ip",
						event->m_info.m_addrRemote.m_port, L"port");

					break;
				}

				if (!askClientConnect(&event->m_info.m_addrRemote))
				{
					networkInterface->CloseConnection(
						event->m_hConn,
						0,
						"connection denied",
						false);

					break;
				}
				
				if (networkInterface->AcceptConnection(event->m_hConn) != k_EResultOK)
				{
					Log::Error(L"Server is unable to deny the connection",
						event->m_info.m_addrRemote.GetIPv4(), L"ip",
						event->m_info.m_addrRemote.m_port, L"port");

					break;
				}

				onClientConnect(event->m_hConn);

				break;
			case k_ESteamNetworkingConnectionState_ClosedByPeer:
				onClientDisconnected(event->m_hConn);

				if (!networkInterface->CloseConnection(
						event->m_hConn,
						1,
						"closed by peer",
						false))
				{
					Log::Warning(L"Tried to close invalid connection handle after connection was closed by peer",
						event->m_info.m_addrRemote.GetIPv4(), L"ip");
				}

				removeConnection(event->m_hConn);

				break;
			case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
				onClientLost(event->m_hConn);

				if (!networkInterface->CloseConnection(
					event->m_hConn,
					1,
					"lost connection",
					false))
				{
					Log::Warning(L"Tried to close invalid connection after lost client",
						event->m_info.m_addrRemote.GetIPv4(), L"ip");
				}

				removeConnection(event->m_hConn);

				break;
			default:
				Log::Information(L"Got unusual new connection status",
					(int) event->m_info.m_eState, L"state");

				break;
			}
		}

		HSteamListenSocket listenSocket;
	};

	class Client
		:
		public ISteamNetworkingSocketsCallbacks,
		public ClientHandler
	{
	public:
		Client()
		{
			connection = k_HSteamNetConnection_Invalid;
		}

		~Client()
		{
			if (connection != k_HSteamNetConnection_Invalid)
				closeConnection(4, "client destructor");
		}

		bool connect(const SteamNetworkingIPAddr& ipAddress)
		{
			networkInterface = SteamNetworkingSockets();

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

			return true;
		}

		bool process() override
		{
			networkInterface->RunCallbacks(this);
			return ClientHandler::process();
		}

	protected:
		void closeConnection(
			const int reasonID,
			const char* reason,
			const bool linger = false)
		{
			if (!networkInterface->CloseConnection(connection, reasonID, reason, linger))
			{
				Log::Error(L"Got invalid connection while closing connection");
			}

			connection = k_HSteamNetConnection_Invalid;
		}

		virtual void onConnectionOpen() = 0;
		virtual void onConnectionLost(const int reason) = 0;
		virtual void onConnectionClosed(const int reason) = 0;

	private:
		void OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* const event) override
		{
			if (connection == event->m_hConn ||
				connection == k_HSteamNetConnection_Invalid)
			{
				Log::Error(L"Got invalid connection in callback",
					(int)connection, L"connection",
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
				onConnectionOpen();

				break;
			case k_ESteamNetworkingConnectionState_ClosedByPeer:
				onConnectionClosed(event->m_info.m_eEndReason);
				closeConnection(5, "closed by peer");

				break;
			case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
				onConnectionLost(event->m_info.m_eEndReason);
				closeConnection(6, "lost connection");

				break;
			default:
				Log::Information(L"Got unusual new connection status",
					(int)event->m_info.m_eState, L"state");

				break;
			}
		}
	};

}

