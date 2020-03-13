#pragma once

#include "NetCore/Client.h"

#include <unordered_map>

namespace Net
{
	struct Request
	{
		virtual void onMessage(
			const MessageID messageID,
			Resource::ReadPipe* const pipe) = 0;

	};

	class OperatorClient
		:
		public ::Net::ClientInterface
	{
		OperatorClient(const SteamNetworkingIPAddr address)
			:
			address(address)
		{
		}

		static OperatorClient* client;

		static sf::Time processInterval;
		static sf::Time processCounter;

	public:
		static void Initialize(
			const SteamNetworkingIPAddr address,
			const sf::Time processInterval = sf::milliseconds(100))
		{
			OperatorClient::processInterval = processInterval;
			client = new OperatorClient(address);
		}

		static void Uninitialize()
		{
			if (client)
				delete client;
		}

	private:
		const SteamNetworkingIPAddr address;

		std::unordered_map<RequestID, Request*> requests;

		void onConnectionOpened() override
		{

		}

		void onConnectionLost(const int reason) override
		{

		}

		void onConnectionClosed(const int reason) override
		{

		}
	};
}
