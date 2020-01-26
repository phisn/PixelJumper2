#pragma once

#include <Client/source/device/RandomDevice.h>
#include <Client/source/logger/Logger.h>
#include <Client/source/resource/pipes/PacketPipe.h>

#include <SFML/Network.hpp>

#define DEV_NET_PORT 9928

namespace Device::Net
{
	class Client
	{
	public:
		Client(const sf::TcpSocket socket)
			:
			socket(socket)
		{
		}

	private:
		sf::TcpSocket socket;
	};

	class Server
	{
	public:
		Server(const int maxClients)
			:
			maxClients(maxClients)
		{
			listener.setBlocking(false);
			clients.reserve(maxClients);
		}

		bool initialize(const int port = DEV_NET_PORT)
		{
			const sf::Socket::Status status = listener.listen(port);
			if (status != sf::Socket::Status::Done)
			{
				Log::Error(L"Failed to set server socket to listen",
					(int) status, L"status", port, L"port");

				return false;
			}

			return true;
		}

		bool pollClient()
		{
			listener.accept();
		}

	private:
		const int maxClients;

		sf::TcpListener listener;
		std::vector<Client> clients;
	};
}

