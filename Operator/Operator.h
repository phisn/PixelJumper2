#pragma once

#include "OperatorClientHandler.h"

namespace Operator::Net
{
	class Operator
		:
		public ::Net::ServerInterface
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

		void process()
		{
			decltype(connections)::iterator iterator = connections.begin();

			while (iterator != connections.end())
			{
				OperatorClientHandler* const clientHandler = *iterator;

				if (clientHandler->getStatus() == OperatorClientHandler::Status::Closing)
				{
					getNetworkInterface()->CloseConnection(
						clientHandler->getConnection(),
						Host::ConnectionClosedReason::Authentication,
						"status is closing",
						false);

					iterator = connections.erase(iterator);
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

		std::vector<OperatorClientHandler*> connections;

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

			if (connections.size() > settings.maxClientCount)
			{
				idleOldConnection();
			}

			connections.emplace_back(
				new OperatorClientHandler(
					connection, 
					settings.authenticationTimeout)
			);
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
			// think about how to idle a connection
			// clienthandler has idle method
			// - clienthandler is removed from operator
			// - clienthandler sends idle message to client
			// - clienthandler later deletes itself
			// alternative idle message by closeconnection 
			// reason (better?)

			decltype(connections)::iterator iterator = connections.begin();
			decltype(connections)::iterator old = iterator;

			while (++iterator != connections.end())
				if ((*iterator)->getAge() > (*old)->getAge())
				{
					old = iterator;
				}


			getNetworkInterface()->CloseConnection(
				(*old)->getConnection(),
				-2,
				"idle connection",
				true);

			delete *old;
			connections.erase(old);
		}
	};
}
