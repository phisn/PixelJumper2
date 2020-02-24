#pragma once

#include <Client/source/device/NetDevice.h>
#include <Operator/source/operator/OperatorClientHandler.h>

namespace Operator::Net
{
	class Operator
		:
		public Device::Net::Server
	{
	public:
		struct Settings
		{
			sf::Uint32 maxClientCount;
			sf::Uint32 authenticationTimeout;
		};

		Operator(const Settings settings)
			:
			settings(settings)
		{
		}

		enum Status
		{
			Starting,
			Running,
			Shuttingdown,
			Shutdown
		};

		void process() override
		{
			Server::process();

			decltype(connections)::iterator iterator = connections.begin();

			while (iterator != connections.end())
			{
				if (iterator->getStatus() == OperatorClientHandler::Status::Closing)
				{
					removeConnection(
						iterator->getConnection(),
						Host::ConnectionClosedReason::Authentication,
						false);

					iterator = connections.erase(iterator);
				}
				else
				{
					iterator->update();
					++iterator;
				}
			}
		}

		Status getStatus() const
		{
			return status;
		}

	private:
		const Settings settings;
		Status status;

		std::vector<OperatorClientHandler> connections;

		bool askClientConnect(SteamNetworkingIPAddr* const ipAddress) override
		{
			Log::Information(L"Client in coming");

			return true;
		}

		void onClientConnect(const HSteamNetConnection connection) override
		{
			Log::Information(L"Client connected");

			if (connections.size() > settings.maxClientCount)
			{
				idleOldConnection();
			}

			connections.emplace_back(connection, settings.authenticationTimeout);
		}

		void onClientDisconnected(const HSteamNetConnection connection) override
		{
			Log::Information(L"Client disconnected");
		}

		void onClientLost(const HSteamNetConnection connection) override
		{
			Log::Information(L"Client lost");
		}

		void idleOldConnection()
		{
			decltype(connections)::iterator iterator = connections.begin();
			decltype(connections)::iterator old = iterator;

			while (++iterator != connections.end())
				if (iterator->getAge() > old->getAge())
				{
					old = iterator;
				}

			// old->close();

			removeConnection(
				old->getConnection(), 
				Host::ConnectionClosedReason::IdleConnection, 
				true);

			connections.erase(old);
		}
	};
}
