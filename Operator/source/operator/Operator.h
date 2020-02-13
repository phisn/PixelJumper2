#pragma once

#include <Client/source/device/NetDevice.h>
#include <Operator/source/operator/RequestHandler.h>

namespace Operator
{
	class Operator
		:
		public Device::Net::Server
	{
	public:
		enum Status
		{
			Starting,
			Running,
			Shuttingdown,
			Shutdown
		};

		Status getStatus() const
		{
			return status;
		}


	private:
		Status status;

		bool askClientConnect(SteamNetworkingIPAddr* const ipAddress) override
		{
			Log::Information(L"Client in coming");

			return true;
		}

		void onClientConnect(const HSteamNetConnection connection) override
		{
			Log::Information(L"Client connected");
			connections.emplace_back(connection, sf::seconds(20));
		}

		void onClientDisconnected(const HSteamNetConnection connection) override
		{
			Log::Information(L"Client disconnected");
		}

		void onClientLost(const HSteamNetConnection connection) override
		{
			Log::Information(L"Client lost");
		}

		std::vector<Net::RequestHandler> connections;
	};
}
