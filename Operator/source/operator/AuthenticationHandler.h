#pragma once

#include <Client/source/device/NetDevice.h>
#include <Client/source/device/EncryptionDevice.h>

#include <Operator/source/database/DatabaseInterface.h>
#include <Operator/source/net/AuthClientMessage.h>
#include <Operator/source/net/AuthOperatorMessage.h>

namespace Operator::Net
{
	class AuthenticationHandler
		:
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

				beginMessage(Host::AuthMessageID::Timeout, 8);
				sendMessage();
			}
		}

		Status getStatus() const
		{
			return status;
		}

	protected:
		Status status = Connecting;
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
			case Client::AuthMessageID::Authenticate:
			{
				Client::AuthenticationMessage message;

				if (!message.load(pipe))
				{
					this->onThreatIdentified(
						messageID,
						L"invalid messagecontent",
						Device::Net::ThreatLevel::Uncommon);

					beginMessage(Host::AuthMessageID::InternalError, 8);
					sendMessage();

					return;
				}

				onAuthenticate(message);

				break;
			}
			case Client::AuthMessageID::Register:
			{
				Client::RegistrationMessage message;

				if (!message.load(pipe))
				{
					this->onThreatIdentified(
						messageID,
						L"invalid messagecontent",
						Device::Net::ThreatLevel::Uncommon);

					beginMessage(Host::AuthMessageID::InternalError, 8);
					sendMessage();

					return;
				}

				onRegistration(message);

				break;
			}
			default:
				// could be a brute-force messageid finder
				this->onThreatIdentified(messageID,
					L"invalid messageid",
					Device::Net::ThreatLevel::Suspicious);

				break;
			}
		}

		void sendCommonMessage(
			Device::Net::MessageID messageID,
			Game::Net::NetworkMessage* const message)
		{
			if (!message->save(beginMessage(messageID)) || !sendMessage())
			{
				Log::Error(L"Unable to construct message",
					(sf::Uint64) messageID, L"messageID");

				status = Disconnecting;
			}
		}

	private:
		void onAuthenticate(const Client::AuthenticationMessage& request)
		{
			Database::User user;

			const Device::Database::ExtractionResult result = Database::Interface::GetUserInfo(
				&user,
				request.username);

			switch (result)
			{
			case Device::Database::ExtractionResult::NotFound:
				beginMessage(Host::AuthMessageID::RejectAuthentication);
				sendMessage();

				onThreatIdentified(
					Client::AuthMessageID::Authenticate,
					L"wrong username",
					Device::Net::ThreatLevel::Suspicious);

				return;
			case Device::Database::ExtractionResult::Error:
				beginMessage(Host::AuthMessageID::InternalError, 8);
				sendMessage();

				return;
			}

			char messageHash[OPERATOR_HASH_SIZE];

			Device::Encryption::HashHashSalt(
				(unsigned char*) messageHash,
				(unsigned char*) user.hash,
				(unsigned char*) user.salt);

			if (memcmp(messageHash, request.content.hash, OPERATOR_HASH_SIZE) != 0)
			{
				beginMessage(Host::AuthMessageID::RejectAuthentication);
				sendMessage();

				return;
			}

			Host::AcceptAuthenticationMessage message;

			if (!Database::Interface::CreatePlayerToken(
					message.authenticationToken,
					user.userID))
			{
				beginMessage(Host::AuthMessageID::InternalError, 8);
				sendMessage();

				return;
			}

			message.playerID = playerID;

			sendCommonMessage(
				Host::AuthMessageID::AcceptAuthentication,
				&message);
			
			status = Connected;
			this->playerID = playerID;

			onClientConnected();
		}

		void onRegistration(const Client::RegistrationMessage& request)
		{
			char hash[OPERATOR_HASH_SIZE];
			char salt[OPERATOR_SALT_SIZE];

			Device::Random::FillRandom(OPERATOR_SALT_SIZE, salt);

			Device::Encryption::HashHashSalt(
				(unsigned char*) hash,
				(unsigned char*) request.content.hash,
				(unsigned char*) salt);

			Resource::PlayerID playerID;

			const Database::Interface::CreatePlayerResult result = Database::Interface::CreateNewPlayer(
				&playerID,
				salt,
				hash,
				request.username,
				request.content.key);

			switch (result)
			{
			case Database::Interface::CreatePlayerResult::UsernameUsed:
				RejectRegistration(Host::RejectRegistrationMessage::UsernameUsed);

				return;
			case Database::Interface::CreatePlayerResult::KeyUsed:
				RejectRegistration(Host::RejectRegistrationMessage::KeyUsed);

				return;
			case Database::Interface::CreatePlayerResult::KeyNotFound:
				RejectRegistration(Host::RejectRegistrationMessage::KeyInvalid);

				return;
			case Database::Interface::CreatePlayerResult::Error:
				beginMessage(Host::AuthMessageID::InternalError);
				sendMessage();

				return;
			}

			Host::AcceptRegistrationMessage message;
			
			if (!Database::Interface::CreatePlayerToken(
					message.authenticationToken,
					playerID))
			{
				beginMessage(Host::AuthMessageID::InternalError, 8);
				sendMessage();

				return;
			}
			
			message.playerID = playerID;
			
			sendCommonMessage(
				Host::AuthMessageID::AcceptRegistration,
				&message);

			status = Connected;
			this->playerID = playerID;

			onClientConnected();
		}

		void RejectRegistration(const Host::RejectRegistrationMessage::Reason reason)
		{
			Host::RejectRegistrationMessage message;
			message.reason = reason;

			sendCommonMessage(
				Host::AuthMessageID::RejectRegistration,
				&message);
		}

		void onTokenAuthentication(const Client::TokenMessage& request)
		{
			Database::User user;

			Device::Database::ExtractionResult result = Database::Interface::GetUserInfo(
				&user,
				request.token);

			switch (result)
			{
			case Device::Database::ExtractionResult::NotFound:

				break;
			case Device::Database::ExtractionResult::Error:
				
				break;
			}
		}
	};
}
