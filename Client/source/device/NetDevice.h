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
	public:
		ClientHandler(const HSteamNetConnection connection)
			:
			connection(connection)
		{
			networkInterface = SteamNetworkingSockets();
		}

		virtual ~ClientHandler()
		{
			// server closes handle
		}

		bool process()
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

		virtual void sendMessage(Resource::WritePipe* const pipe)
		{
			networkInterface->SendMessageToConnection(
				connection,
				pipe->);
		}

	private:
		ISteamNetworkingSockets* networkInterface;
		const HSteamNetConnection connection;
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

		// ask weather accept the connection or
		// deny. allows denial based on ipaddress
		virtual bool askClientConnect(SteamNetworkingIPAddr* const ipAddress) = 0;
		virtual void onClientConnect(const HSteamNetConnection connection) = 0;

		virtual bool onClientDisconnected(const HSteamNetConnection connection) = 0;
		virtual bool onClientLost(const HSteamNetConnection connection) = 0;

		bool existsConnection(const HSteamNetConnection connection)
		{
			for (HSteamNetConnection i : connections)
				if (connection == i)
				{
					return true;
				}

			return false;
		}

		std::vector<HSteamNetConnection> connections;

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
						event->m_info.m_addrRemote.m_port);

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

				break;
			default:
				Log::Information(L"Got unusual new connection status",
					(int) event->m_info.m_eState, L"state");

				break;
			}
		}

		HSteamListenSocket listenSocket;
	};
}

