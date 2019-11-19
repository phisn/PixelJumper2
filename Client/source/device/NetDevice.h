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

		enum Type : sf::Uint8
		{
			Neutral,

			Syn,
			SynAck,

			Ack

		} type;

		enum Response : sf::Uint8
		{
			Success,
			Timeout,
			UserAlreadyExists,
			UserDoesNotExists,
			InvalidSequence

		} response;

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
		enum Status
		{
			Pending,
			Error,

			Connected,
			Disconnected,
			Timeout
		};

		// normally same for a group of clients
		struct Settings
		{
			sf::Time softTimeout;	// re send after time
			int hardTimeout;		// timeout after N re send
		};

		Client(
			const Settings settings,
			const Target target,
			const ClientId id,
			const Sequence sequence)
			:
			settings(settings),
			target(target),
			id(id),
			externalSequence(sequence)
		{
			internalSequence = Random::MakeRandom<Sequence>();
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

		bool PushMessage(MessageWrite* const message)
		{
			Header header;

			header.type = Header::Neutral;
			header.sequence = internalSequence + 1;
			header.response = Header::Success;

			if (packets.size() == 0 && !Net::PushPacket(message, target))
			{
				status = Error;
				return false;
			}

			packets.emplace_back(
				message,
				target,
				header.sequence);

			++internalSequence;

			return true;
		}

		bool HandleMessage(const Header header, MessageRead* const message)
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

				packets.pop_front();

				logicCounter = sf::microseconds(0);
				timeoutCounter = 0;

				if (packets.size() != 0)
				{
					resendPacket();
				}

				return true;
			case Header::Syn:
			{
				Header header;

				header.clientId;
				header.sequence;
				header.type = Header::SynAck;
				header.response = Header::Success;

				// send synack
			}
				return true;
			}


			// send ack
		}

		Status getStatus() const
		{
			return status;
		}

		ClientId getId() const
		{
			return id;
		}

	private:
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

		std::deque<PacketBuffer> packets;

		sf::Time logicCounter;
		int timeoutCounter = 0;
	};

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

			while (PollPacket(&messageBuffer))
			{
				Header header;

				if (!header.make(&messageBuffer))
				{
					Log::Error(L"Failed to make message",
						clients.size(), L"csize");

					continue;
				}

				Client* client = findClientOrNull(header.clientId);
				
				if (client == NULL)
				{
					if (messageBuffer.getType() == MessageType::NetDevice)
					{
						// has to be syn
						if (header.type != Header::Syn)
						{
							Log::Error(L"Got invalid client without syn header",
								header.clientId, L"cid",
								header.sequence, L"seq",
								(int) header.type, L"type",
								clients.size(), L"csize");

							continue;
						}

						if (clients.size() >= settings.maxClients)
						{
							Log::Error(L"Cant accept client because max client number reached",
								clients.size(), L"csize",
								header.clientId, L"cid",
								header.sequence, L"seq");

							continue;
						}

						client = new Client(
							// re send after 50ms
							// timeout after 1000ms
							Client::Settings{
								sf::microseconds(50'000),
								20
							},
							messageBuffer.getTarget(),
							header.clientId,
							header.sequence);

						clients.push_back(client);
					}
					else
					{
						Log::Error(L"");

						continue;
					}
				}

				client->HandleMessage(header, &messageBuffer);
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

		const Settings settings;
		MessageRead messageBuffer;

		std::vector<Client*> clients;
	};

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
