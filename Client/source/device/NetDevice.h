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

	bool Initialize(const unsigned short port);
	void Uninitialize();

	bool Isinitialized();

	struct Target
	{
		unsigned short port;
		sf::IpAddress address;
	};

	struct MessageRead
		:
		Resource::PacketReadPipe
	{
	public:
		sf::Socket::Status pullPacket(sf::UdpSocket* const socket)
		{
			const sf::Socket::Status status = socket->receive(packet, target.address, target.port);

			if (status == sf::Socket::Done && !readValue(&packetId))
			{
				return sf::Socket::Error;
			}

			return status;
		}

		MessageRead* steal()
		{
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
		Target target;
		PacketId packetId;
	};

	class MessageReadBuffer
	{
	public:
		MessageReadBuffer()
		{
			message = new MessageRead();
		}

		~MessageReadBuffer()
		{
			delete message;
		}

		MessageRead* steal()
		{
			MessageRead* const temp = message;
			message = new MessageRead();
			return temp;
		}

		MessageRead* operator->() const
		{
			return message;
		}

		MessageRead* operator*() const
		{
			return message;
		}

	private:
		MessageRead* message;
	};

	struct MessageWrite
		:
		Resource::PacketWritePipe
	{
		friend class Client;

	public:
		bool load()
		{
			const PacketId packetId = Device::Random::MakeRandom<PacketId>();
			reset();
			return writeValue(&packetId);
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

		// error >= 128
		enum Type : sf::Uint8
		{
			Reliable,
			Unreliable,

			Syn,
			SynAck,
			Ack,

			Error_Timeout = 0b1000000,
			Error_UserAlreadExists,
			Error_UserDoesNotExist,
			Error_InvalidSequence
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


	// packets cant be received double
	// (actually can but it is extremly 
	// rare [never happend before]) packets 
	// have reliable data
	bool PollPacket(MessageRead* const messsage);
	bool PushPacket(
		MessageWrite* const message,
		const Target target);

	class Client
	{
	public:
		// normally same for a group of clients
		struct Settings
		{
			sf::Time softTimeout = sf::milliseconds(20);	// re send after time
			int hardTimeout = 50;		// timeout after N re send
		};

	private:
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
		enum Status
		{
			Pending,
			Error,

			Connected,
			Disconnected,
			Timeout
		};

		Client(
			const Settings settings,
			const Target target,
			const ClientId id,
			const Sequence sequence)
			:
			Client(settings, target)
		{
			this->id = id;
			this->externalSequence = sequence;
		}

		Client(
			const Settings settings,
			const Target target)
			:
			id(NULL),
			settings(settings),
			target(target)
		{
			internalSequence = Random::MakeRandom<Sequence>();
			pushReliableMessageNet(Header::Syn);
		}

		~Client()
		{
			for (const PacketBuffer& packet : packets)
				packet.message->deref();
		}

		void disconnect()
		{
			status = Disconnected;
		}

		virtual void onLogic(const sf::Time time)
		{
			if (packets.size() != 0
				&& (logicCounter += time) > settings.softTimeout)
			{
				if (++timeoutCounter > settings.hardTimeout)
				{
					status = Timeout;
					timeoutCounter = 0;
				}
				else
				{
					resendPacket();
				}

				logicCounter = sf::microseconds(0);
			}

			// - handle missing packets answers 
			// (ack) after timeout [primary]
			// - handle missing synack or ack
			// answers

			// ! double packets are already handled 
			// by low level net device
		}

		bool pushUnreliableMessage(MessageWrite* const message)
		{
			Header header;

			header.clientId = id;
			header.type = Header::Type::Unreliable;
			header.sequence = unreliableSequence++;

			if (!message->writeValue(&header) ||
				!Net::PushPacket(message, target))
			{
				status = Error;
				return false;
			}

			return true;
		}

		bool pushReliableMessage(MessageWrite* const message)
		{
			return pushReliableMessageRaw(message, Header::Type::Reliable);
		}

		bool handleMessage(const Header header, MessageRead* const message)
		{
			switch (header.type)
			{
			case Header::Ack:
				if (packets.size() == 0)
				{
					Log::Error(L"Got ack while empty packetbuffer" + logSuffix(),
						header.sequence, L"hseq",
						externalSequence, L"eseq",
						internalSequence, L"iseq");
					status = Error;

					return false;
				}

				if (header.sequence != packets[0].sequence)
				{
					// send invalid sequence

					Log::Error(L"Got packet with invalid sequence" + logSuffix(),
						header.sequence, L"hseq",
						packets[0].sequence, L"pseq",
						externalSequence, L"eseq",
						internalSequence, L"iseq");
					status = Error;

					return false;
				}

				packets[0].message->deref();
				packets.pop_front();

				logicCounter = sf::microseconds(0);
				timeoutCounter = 0;

				// if packet is waiting send next packet
				if (packets.size() != 0)
				{
					resendPacket();
				}

				return true;
			case Header::Syn:
				pushReliableMessageNet(Header::SynAck);

				return true;
			}

			if (header.sequence == externalSequence)
			{
				onMessage(message);
			}

			pushReliableMessageNet(Header::Ack);

			return true;
		}

		Status getStatus() const
		{
			return status;
		}

		ClientId getId() const
		{
			return id;
		}

	protected:
		virtual void onMessage(MessageRead* const message) = 0;

	private:
		bool pushReliableMessageNet(const Header::Type type)
		{
			MessageWrite* const message = new MessageWrite();

			if (message->load() && pushReliableMessageRaw(message, type))
			{
				message->resign();
				return true;
			}
			else
			{
				delete message;
				return false;
			}
		}

		bool pushReliableMessageRaw(
			MessageWrite* const message,
			const Header::Type type)
		{
			Header header;

			header.clientId = id;
			header.type = type;
			header.sequence = internalSequence;

			if (packets.size() == 0 && !Net::PushPacket(message, target))
			{
				status = Error;
				return false;
			}

			message->ref();

			packets.emplace_back(
				message,
				target,
				header.sequence);

			++internalSequence;

			return true;
		}

		void resendPacket()
		{
			if (!PushPacket(packets[0].message, packets[0].target))
			{
				Log::Error(L"Failed to resend packet" + logSuffix(),
					packets[0].sequence, L"pseq",
					internalSequence, L"iseq",
					externalSequence, L"eseq");

				status = Error;
			}
		}

		std::wstring logSuffix()
		{
			return Log::Convert(id, L"cid",
				target.address.toInteger(), L"ip",
				target.port, L"port",
				status, L"status");
		}

		Status status = Pending;
		const Settings settings;

		ClientId id;
		Target target;

		Sequence internalSequence;
		Sequence externalSequence;

		// unreliable sequence starts at 0 and
		// is only used to detect the amount of
		// packet loss
		Sequence unreliableSequence;

		std::deque<PacketBuffer> packets;
		std::deque<MessageRead*> messages;

		sf::Time logicCounter;
		int timeoutCounter = 0;
	};

	template <typename ClientT>
	class Server
	{
	public:
		struct Settings
		{
			int maxClients;
		};

		Server(const Settings settings)
			:
			settings(settings)
		{
		}

		virtual void onLogic(const sf::Time time)
		{
			for (Client* const client : clients)
				client->onLogic(time);

			while (PollPacket(*messageBuffer))
			{
				Header header;

				if (!header.make(*messageBuffer))
				{
					Log::Error(L"Failed to make message",
						clients.size(), L"csize");

					continue;
				}

				Client* client = findClientOrNull(header.clientId);

				if (client == NULL)
				{
					// has to be syn
					if (header.type != Header::Syn)
					{
						Log::Error(L"Got invalid client without syn header",
							header.clientId, L"cid",
							header.sequence, L"seq",
							(int)header.type, L"type",
							clients.size(), L"csize");

						// send error

						continue;
					}

					if (clients.size() >= settings.maxClients)
					{
						Log::Error(L"Cant accept client because max client number reached",
							clients.size(), L"csize",
							header.clientId, L"cid",
							header.sequence, L"seq");

						// send error

						continue;
					}

					client = new Client(
						// re send after 50ms
						// timeout after 1000ms
						Client::Settings{
							sf::microseconds(50'000),
							20
						},
						messageBuffer->getTarget(),
						createClientId(),
						header.sequence);

					clients.push_back(client);
				}

				client->handleMessage(header, &messageBuffer);
			}
		}

		virtual void cleanupClients()
		{
			for (decltype(clients)::iterator i = clients.begin(); i != clients.end(); ++i)
				if ((*i)->getStatus() != Client::Connected)
				{
					clients.erase(i);
				}
		}

		const std::vector<Client*>& getClients() const
		{
			return clients;
		}

	protected:
		virtual void onClient(Client* const client) = 0;
		virtual void onClientRemove(Client* const client) = 0;

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

		ClientId createClientId() const
		{
			ClientId clientId;

			while (true)
			{
				clientId = Random::MakeRandom<ClientId>();

				for (Client* const client : clients)
					if (client->getId() == clientId)
					{
						goto GET_CLIENT_ID_RETRY;
					}

				return clientId;
			GET_CLIENT_ID_RETRY:
			}
		}

		const Settings settings;
		MessageReadBuffer messageBuffer;

		std::vector<Client*> clients;
	};
}

