#pragma once

#include <Client/source/device/NetDevice.h>
#include <Operator/source/operator/ElementConnection.h>

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
			return true;
		}

		void onClientConnect(const HSteamNetConnection connection) override
		{
			connections.emplace_back(connection);
		}

		void onClientDisconnected(const HSteamNetConnection connection) override
		{
		}

		void onClientLost(const HSteamNetConnection connection) override
		{
		}

		std::vector<ElementConnection> connections;
	};
}
