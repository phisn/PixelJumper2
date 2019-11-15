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

			if (status == sf::Socket::Done && !(
					readValue(&packetId) && readValue(&type) ))
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

		PacketId getPacketId() const
		{
			return packetId;
		}

	private:
		MessageType type;
		Target target;
		PacketId packetId;
	};

	struct MessageWrite
		:
		Resource::PacketWritePipe
	{
		friend class Client;

	public:
		bool load(const MessageType type)
		{
			const PacketId packetId = Device::Random::MakeRandom<PacketId>();

			reset();
			
			return writeValue(&packetId)
				&& writeValue(&type);
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

		// used for owners to resign their ownership
		// and "free" this object. because MessageWrite
		// can still be used in client as buffer (common)
		// this object should not be delete at resign if
		// a ref is still hold (called by client). this
		// method is used, because we wont use sharedptr
		void resign()
		{
			if (counter == 0)
			{
				delete this;
			}
			else
			{
				resigned = true;
			}
		}

	private:
		// only used in client (access by friend) for
		// commiting to this object
		void ref()
		{
			++counter;
		}

		// called when client (access by friend) gets
		// ack from packet (buffer packet released)
		void deref()
		{
			if (--counter == 0 && resigned)
			{
				delete this;
			}
		}

		bool resigned = false;
		int counter = 0;
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
				MessageWrite* const message,
				const Target target,
				const Sequence sequence)
				:
				message(message),
				target(target),
				sequence(sequence)
			{
			}

			MessageWrite* const message;
			const Target target;
			const Sequence sequence;
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
			const Target target,
			const ClientId id,
			const Sequence sequence)
			:
			target(target),
			id(id),
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

		bool PushPacket(MessageWrite* const message)
		{
			Header header;

			header.type = Header::Neutral;
			header.sequence = internalSequence + 1;

			if (Net::PushPacket(message, target))
			{
				return false;
			}

			packets.emplace_back(
				message,
				target,
				header.sequence);

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

		std::deque<PacketBuffer> packets;
	};

	class Server
	{
	public:
		Server(const int maxClients)
		{
		}

		virtual void onLogic(const sf::Time time)
		{
			for (Client* const client : clients)
				client->onLogic(time);

			while (PollPacket(&messageBuffer))
			{
				Header header;

				if (!header.make(&messageBuffer))
				{
					continue;
				}

				switch (messageBuffer.getType())
				{
				case MessageType::NetDevice:
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
						header.sequence);

					break;
				case MessageType::Operator:
					// forward messages to client

					break;
				case MessageType::Connection:
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
		MessageRead messageBuffer;

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

	// packets cant be received double
	// (actually can but it is extremly 
	// rare [never happend before]) packets 
	// have reliable data
	bool PollPacket(MessageRead* const messsage);
	bool PushPacket(
		MessageWrite* const message,
		const Target target);

/*
*/
}
