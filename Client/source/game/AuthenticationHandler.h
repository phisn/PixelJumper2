#pragma once

#include <Client/source/game/net/ClientMessage.h>
#include <Client/source/game/net/ServerMessage.h>

#include <Client/source/operator/OperatorAccess.h>

namespace Game::Net
{
	/*
	
		1. Client requests operator
		   auth token
		2. Operator checks credentials
		   and sends auth token
		3. Client hashes auth token
		   with random salt to auth
		4. Client requests auth at
		   simulator
		5. AuthHandler requests operator 
		   confirmation
		6. Operator confirms auth
		   playerid
		7. AuthHandler accept client 
		   auth
		8. Client requests start of 
		   simulation
		9. Simulation accepts request 
		   and starts
	
	*/

	struct AuthenticationMessage
	{
		Operator::ClientIdentifactor identification;
	};

	// authhandler as base for remoteconnection
	class AuthenticationHandler
		:
		public Operator::OperatorAccessClient,
		public Device::Net::ClientHandler
	{
	public:
		enum Status
		{
			// user is not authenticated and
			// identification not known
			Connecting,
			// user is authenticated and
			// identification known
			Connected,

			// user is still connected or
			// connecting but connection will
			// be closed soon. identification
			// may not be known
			Disconnecting,
			// user is not connected. identification
			// may not be known
			Disconnected,
		};

		// timeout in ticks in 100ms
		AuthenticationHandler(const sf::Uint32 timeout = 100)
			:
			timeout(timeout)
		{
		}

		// has to override and only be called
		// when status == Connecting
		virtual void update()
		{
			if (--timeout == 0)
			{
				status = Disconnecting;

				if (playerID == NULL)
				{
					beginMessage(Host::AuthenticationMessageID::Timeout, 8);
					sendMessage();
				}
				else
				{
					beginMessage(Host::AuthenticationMessageID::OperatorTimeout, 8);
					sendMessage();
				}
			}
		}

		Status getStatus() const
		{
			return status;
		}

	protected:
		Status status;
		Resource::PlayerID playerID = NULL;

		// use timeout in specializations when
		// needed to abuse unused memory after
		// status connecting
		sf::Uint32 timeout;

		virtual void onClientConnected() = 0;

		virtual void onMessage(
			const Device::Net::MessageID messageID, 
			Resource::ReadPipe* const pipe) override
		{
			switch (messageID)
			{
			case Client::AuthenticationMessageID::Authenticate:
				Client::AuthenticationMessage message;

				// playerid cant be null to identify operator
				// timeout
				if (!message.load(pipe) && message.playerID != NULL)
				{
					this->onThreatIdentified(
						messageID,
						L"invalid messagecontent",
						Device::Net::ThreatLevel::Uncommon);

					beginMessage(Host::AuthenticationMessageID::InvalidAuthentication, 8);
					sendMessage();

					return;
				}

				Operator::RegisterClient(&message.identificator, this);
				playerID = message.playerID;

				break;
			default:
				// could be a brute-force messageid finder
				this->onThreatIdentified(messageID,
					L"invalid messageid",
					Device::Net::ThreatLevel::Suspicious);

				break;
			}
		}

		void safeMessageProcess(
			Device::Net::MessageID messageID,
			NetworkMessage* const message,
			Resource::WritePipe* const pipe)
		{
			if (!message->save(pipe))
			{
				Log::Error(L"Unable to construct message",
					(sf::Uint64) messageID, L"messageID");

				status = Disconnecting;
			}
			else
			{
				sendMessage();
			}
		}

	private:
		void onOperatorClientConnected(const Resource::PlayerID playerID)
		{
			if (this->playerID != playerID)
			{
				beginMessage(Host::AuthenticationMessageID::InvalidPlayerID, 8);
				sendMessage();

				status = Disconnecting;
			}
			else
			{
				beginMessage(Host::AuthenticationMessageID::AcceptAuthentication, 8);
				sendMessage();

				status = Connected;

				onClientConnected();
			}
		}

		void onOperatorClientRejected() override
		{
			beginMessage(Host::AuthenticationMessageID::InvalidToken, 8);
			sendMessage();

			status = Disconnecting;
			onThreatIdentified(
				Client::AuthenticationMessageID::Authenticate,
				L"invalid token",
				Device::Net::ThreatLevel::Suspicious);
		}

		void onOperatorClientDisconnected() override
		{
			beginMessage(Host::AuthenticationMessageID::OperatorDisconnect);
			sendMessage();

			status = Disconnected;
		}
	};
}
