#pragma once

#include "Logger/Logger.h"

#include "steam/steamnetworkingsockets.h"
#include "steam/isteamnetworkingutils.h"

#include "SFML/Main.hpp"

namespace Net
{
	typedef sf::Uint32 MessageID;

	enum class ThreatLevel
	{
		Uncommon,
		Suspicious,
		Malicious
	};

	enum class SendFailure
	{
		Save,
		Send,
		Size
	};

	namespace Core
	{
		bool Initialize();
		void Uninitialize();

		const SteamNetworkingIdentity* GetIdentity();
		
		void Process();
	};

	namespace Util
	{
		const std::wstring ConvertIPAddress(SteamNetworkingIPAddr* const address);
	}

	struct ClientInterface
		:
		public ISteamNetworkingSocketsCallbacks
	{
	public:
		virtual ~ClientInterface();

		bool connect(const SteamNetworkingIPAddr& address);

		HSteamListenSocket getSocket() const
		{
			return socket;
		}

	protected:
		ISteamNetworkingSockets* getNetworkInterface() const;

		virtual void onConnectionOpened() = 0;
		virtual void onConnectionLost(const int reason) = 0;
		virtual void onConnectionClosed(const int reason) = 0;

	private:
		HSteamListenSocket socket = k_HSteamListenSocket_Invalid;

		void OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* const event) override
		{
			Log::Information(L"onsteamnetconnectionstatuschanged",
				(unsigned long long) event->m_hConn, L"connection",
				(unsigned long long) event->m_info.m_hListenSocket, L"listenconnection");

			if (socket != event->m_hConn &&
				socket != k_HSteamNetConnection_Invalid)
			{
				Log::Error(L"Got invalid connection in callback",
					(int)event->m_hConn, L"iconnection",
					(int) socket, L"connection",
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
				
				getNetworkInterface()->CloseConnection(
					socket,
					0,
					"closed by peer",
					false);
				socket = k_HSteamNetConnection_Invalid;

				break;
			case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
				onConnectionLost(event->m_info.m_eEndReason);
				
				getNetworkInterface()->CloseConnection(
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
	};

	class ServerInterface
		:
		public ISteamNetworkingSocketsCallbacks
	{
	public:
		virtual ~ServerInterface();

		bool initialize(const sf::Uint16 port = 9927);

		HSteamListenSocket getSocket() const
		{
			return socket;
		}

	protected:
		ISteamNetworkingSockets* getNetworkInterface() const;

		virtual bool askClientConnect(SteamNetConnectionStatusChangedCallback_t* const event) = 0;
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
				if (!askClientConnect(event))
				{
					getNetworkInterface()->CloseConnection(
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
						(int) result, L"result",
						Util::ConvertIPAddress(&event->m_info.m_addrRemote), L"ip");

					getNetworkInterface()->CloseConnection(
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

				getNetworkInterface()->CloseConnection(
					event->m_hConn,
					1,
					"closing failed to accept connection",
					false);

				break;
			case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
				onClientLost(event->m_hConn);

				getNetworkInterface()->CloseConnection(
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

		HSteamListenSocket socket = k_HSteamListenSocket_Invalid;
	};
}
