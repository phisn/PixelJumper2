#pragma once

#include "OperatorClientHandler.h"

#include "NetCore/message/OperatorCloseReason.h"

namespace Operator
{
	class OperatorCore
		:
		public ::Net::ServerInterface
	{
	public:
		struct Settings
		{
			sf::Uint32 maxClientCount;
			sf::Uint32 authenticationTimeout;
		};

		OperatorCore(const Settings settings)
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

		void process()
		{
			decltype(clients)::iterator iterator = clients.begin();

			while (iterator != clients.end())
			{
				OperatorClientHandler* const clientHandler = *iterator;

				if (clientHandler->getStatus() == OperatorClientHandler::Status::Closing)
				{
					getNetworkInterface()->CloseConnection(
						clientHandler->getConnection(),
						::Net::OperatorCloseReason::ConnectionClosed,
						"status is closing",
						false);

					iterator = clients.erase(iterator);
				}
				else
				{
					clientHandler->update();
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

		std::vector<OperatorClientHandler*> clients;

		bool askClientConnect(SteamNetConnectionStatusChangedCallback_t* const event) override
		{
			Log::Information(L"Client connecting",
				(unsigned long long) event->m_hConn, L"handle",
				::Net::ConvertIPAddress(&event->m_info.m_addrRemote), L"address");

			return true;
		}

		void onClientConnect(const HSteamNetConnection connection) override
		{
			Log::Information(L"Client connected");

			if (clients.size() > settings.maxClientCount)
			{
				idleOldConnection();
			}

			clients.emplace_back(
				new OperatorClientHandler(
					connection, 
					settings.authenticationTimeout)
			);
		}

		void onClientDisconnected(const HSteamNetConnection connection) override
		{
			Log::Information(L"Client disconnected");
			removeClient(connection);
		}

		void onClientLost(const HSteamNetConnection connection) override
		{
			Log::Information(L"Client lost");
			removeClient(connection);
		}

		void removeClient(const HSteamNetConnection connection)
		{
			decltype(clients)::iterator client = std::find_if(
				clients.begin(),
				clients.end(),
				[connection](OperatorClientHandler* client) -> bool
				{
					return client->getConnection() == connection;
				});

			if (client == clients.end())
			{
				Log::Error(L"got invalid connection in removeclient",
					(long long) connection, L"connection");
			}
			else
			{
				clients.erase(client);
			}
		}

		void idleOldConnection()
		{
			// think about how to idle a connection
			// clienthandler has idle method
			// - clienthandler is removed from operator
			// - clienthandler sends idle message to client
			// - clienthandler later deletes itself
			// alternative idle message by closeconnection 
			// reason (better?)

			decltype(clients)::iterator iterator = clients.begin();
			decltype(clients)::iterator old = iterator;

			while (++iterator != clients.end())
				if ((*iterator)->getAge() > (*old)->getAge())
				{
					old = iterator;
				}


			getNetworkInterface()->CloseConnection(
				(*old)->getConnection(),
				::Net::OperatorCloseReason::IdleConnection,
				"idle connection",
				true);

			delete *old;
			clients.erase(old);
		}
	};
}
