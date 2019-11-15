#pragma once

#include <Client/source/device/RandomDevice.h>

#include <Client/source/logger/Logger.h>

#include <Client/source/resource/pipes/PacketPipe.h>

#include <SFML/Network.hpp>

#include <queue>

namespace Device::Net
{
	typedef sf::Uint64 PacketId;
	typedef sf::Uint32 ClientId;
	typedef sf::Uint32 Sequence;

	struct Target
	{
		unsigned short port;
		sf::IpAddress address;
	};

	enum MessageType
	{
		Invalid = 0,

		NetDevice,
		Operator,
		Connection,

		Extended, // reserved

		_Length
	} type;

	struct MessageRead
		:
		Resource::PacketReadPipe
	{
	public:
		sf::Socket::Status pullPacket(sf::UdpSocket* const socket)
		{
			const sf::Socket::Status status = socket->receive(packet, target.address, target.port);

			if (status == sf::Socket::Done && !readValue(&type))
			{
				return sf::Socket::Error;
			}

			return status;
		}

		MessageType getType() const
		{
			return type;
		}

		const Target& getTarget() const
		{
			return target;
		}

	private:
		MessageType type;
		Target target;
	};

	struct MessageWrite
		:
		Resource::PacketWritePipe
	{
	public:
		bool load(const MessageType type)
		{
			reset();
			return writeValue(&type);
		}

		sf::Socket::Status pushPacket(
			sf::UdpSocket* const socket,
			const Target target)
		{
			return socket->send(
				packet,
				target.address,
				target.port);
		}
	};

	struct Header
	{
		ClientId clientId;

		enum Type
		{
			Neutral,

			Syn,
			SynAck,
			Ack

		} type;

		Sequence sequence;

		bool make(Resource::ReadPipe* const pipe)
		{
			return pipe->readValue(&clientId)
				&& pipe->readValue(&type)
				&& pipe->readValue(&sequence);
		}

		bool save(Resource::WritePipe* const pipe)
		{
			return pipe->writeValue(&clientId)
				&& pipe->writeValue(&type)
				&& pipe->writeValue(&sequence);
		}
	};

	class Client
	{
		struct PacketBuffer
		{
			PacketBuffer(
				const Target target,
				const Sequence sequence,
				const sf::Packet packet)
				:
				target(target),
				sequence(sequence),
				packet(packet)
			{
			}

			const Target target;
			const Sequence sequence;
			const sf::Packet packet;
		};

	public:
		enum Type
		{
			Pending,
			Error,

			Connected,
			Disconnected,
			Timeout
		};

		Client(
			const ClientId id,
			const PacketContext context,
			const Sequence sequence)
			:
			id(id),
			ipAddress(context.address),
			port(context.port),
			externalSequence(sequence)
		{
			internalSequence = Random::MakeRandom<Sequence>();

			if (!true) // send packet
			{
				type = Error;
			}
		}

		void disconnect()
		{
			type = Disconnected;
		}

		virtual void onLogic(const sf::Time time)
		{
			// - handle missing packets answers 
			// (ack) after timeout [primary]
			// - handle missing synack or ack
			// answers

			// ! double packets are already handled 
			// by low level net device
		}

		bool PushPacket()
		{
			Header header;

			header.type = Header::Neutral;
			header.sequence = internalSequence + 1;

			if (Net::PushPacket(target.address, target.port))
			{
				return false;
			}

			PacketContext context;

			packets.emplace_back(

				header,
				GetReadPipe()->getPacket()
			);

			++internalSequence;

			return true;
		}

		Type getType() const
		{
			return type;
		}

		ClientId getId() const
		{
			return id;
		}

	private:
		Sequence internalSequence;
		Sequence externalSequence;

		Type type = Pending;

		ClientId id;
		Target target;

		std::deque<PacketContext> packets;
	};

	class Server
	{
	public:
		Server(const int maxClients)
		{
		}

		virtual void onLogic(const sf::Time time)
		{
			PacketContext context;

			for (Client* const client : clients)
				client->onLogic(time);

			while (PollPacket(&context))
			{
				Header header;

				if (!header.make(GetReadPipe()))
				{
					continue;
				}

				switch (context.type)
				{
				case PacketContext::NetDeviceMessage:
					if (Client * const client = findClientOrNull(header.clientId); client != NULL)
					{
						// ...
						// forward messages to client
						// - client does assume packet lost on 
						// multiple messages (for all types)

						break;
					}

					// has to be syn
					if (header.type != Header::Syn)
					{
						break;
					}

					Client* const client = new Client(
						header.clientId,
						context,
						header.sequence);

					break;
				case PacketContext::OperatorMessage:
					// forward messages to client

					break;
				case PacketContext::ConnectionMessage:
					// forward messages to client

					break;
				}
			}
		}

		virtual void cleanupClients()
		{
			for (decltype(clients)::iterator i = clients.begin(); i != clients.end(); ++i)
				if ((*i)->getType() != Client::Connected)
				{
					clients.erase(i);
				}
		}

		const std::vector<Client*>& getClients() const
		{
			return clients;
		}

	private:
		Client* findClientOrNull(const ClientId clientId) const
		{
			for (Client* const client : clients)
				if (client->getId() == clientId)
				{
					return client;
				}

			return NULL;
		}

		std::vector<Client*> clients;
	};

	bool Initialize(const unsigned short port);
	void Uninitialize();

	bool Isinitialized();

	bool _PollPacket(MessageRead* const messsage);
	bool _PushPacket(
		MessageWrite* const message,
		const Target target);

/*
	// packets cant be received double
	// (actually can but it is extremly 
	// rare [never happend before])
	// packets have reliable data
	bool PollPacket(PacketContext* const context);
	Resource::PacketReadPipe* GetReadPipe();

	// automatically clears packet
	void RegisterPacket(const PacketContext::Type type);
	// packets must have reliable data
	bool PushPacket(
		const sf::IpAddress ip,
		const unsigned short port);
	Resource::PacketWritePipe* GetWritePipe();
*/
}
