#pragma once

#include "Logger/Logger.h"

#include "steam/steamnetworkingsockets.h"
#include "steam/isteamnetworkingutils.h"

#include "SFML/Main.hpp"

namespace Net
{
	typedef sf::Uint32 MessageID;
	typedef sf::Uint64 RequestID;

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

	const std::wstring ConvertIPAddress(SteamNetworkingIPAddr* const address);

	struct ClientInterface
		:
		public ISteamNetworkingSocketsCallbacks
	{
	public:
		virtual ~ClientInterface();

		bool connect(const SteamNetworkingIPAddr& address);
		void OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* const event) override;

		HSteamListenSocket getSocket() const
		{
			return socket;
		}

		virtual bool disconnect(
			const int reason = 0,
			const char* const message = "disconnect",
			bool linger = false);

		bool isConnected() const
		{
			return socket == k_HSteamListenSocket_Invalid;
		}

	protected:
		ISteamNetworkingSockets* getNetworkInterface() const;

		virtual void onConnectionOpened() = 0;
		virtual void onConnectionLost(const int reason) = 0;
		virtual void onConnectionClosed(const int reason) = 0;

	private:
		HSteamListenSocket socket = k_HSteamListenSocket_Invalid;
	};

	class ServerInterface
		:
		public ISteamNetworkingSocketsCallbacks
	{
	public:
		virtual ~ServerInterface();

		bool initialize(const sf::Uint16 port = 9927);
		bool uninitialize();
		void OnSteamNetConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* const event) override;

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

		HSteamListenSocket socket = k_HSteamListenSocket_Invalid;
	};
}
