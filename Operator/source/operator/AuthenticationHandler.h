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
			Authenticating,
			// user is authenticated and
			// identification known
			Authenticated,

			// user is still connected or
			// connecting but connection will
			// be closed soon. identification
			// may not be known
			Disconnecting,
			// user is not connected. identification
			// may not be known
			Disconnected,
		};

		AuthenticationHandler(
			const HSteamNetConnection connection,
			const sf::Uint32 timeout = 100)
			:
			ClientHandler(connection),
			timeout(timeout)
		{
		}

		virtual void update()
		{
			if (status == Authenticating && --timeout == 0)
			{
				status = Disconnecting;

				beginMessage(Host::AuthMessageID::Timeout, 8);
				sendMessage();

				return;
			}

			if (!process())
			{
				status = Disconnecting;

				// think about removing this because
				// it has a very high chance to fail
				// (gurranted?)
				beginMessage(Host::AuthMessageID::InternalError, 8);
				sendMessage();

				return;
			}

			++age;
		}

		void close()
		{
			if (status == Authenticated)
			{
				beginMessage(Host::AuthMessageID::IdleConnection);
				sendMessage();
			}
		}

		Status getStatus() const
		{
			return status;
		}

		bool isAuthenticated() const
		{
			return status == Authenticated;
		}

		sf::Uint32 getAge() const
		{
			return age;
		}

	protected:
		using ClientHandler::process;

		Status status = Authenticating;
		UserID userID = NULL;

		// use timeout in specializations when
		// needed to abuse unused memory after
		// status connecting
		sf::Uint32 timeout;
		sf::Uint32 age = 0;

		virtual void onClientAuthenticated() = 0;

		virtual void onMessage(
			const Device::Net::MessageID messageID,
			Resource::ReadPipe* const pipe) override
		{
			Log::Information(L"got message", messageID, L"messageID");

			switch (messageID)
			{
			case Client::AuthMessageID::Authenticate:
			{
				Client::AuthenticationMessage message;

				if (loadCommonMessage(messageID, &message, pipe))
				{
					return;
				}

				onAuthenticate(message);

				break;
			}
			case Client::AuthMessageID::Register:
			{
				Client::RegistrationMessage message;

				if (loadCommonMessage(messageID, &message, pipe))
				{
					return;
				}

				onRegistration(message);

				break;
			}
			case Client::AuthMessageID::Token:
			{
				Client::TokenMessage message;

				if (loadCommonMessage(messageID, &message, pipe))
				{
					return;
				}

				onTokenAuthentication(message);

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

		bool loadCommonMessage(
			const Device::Net::MessageID messageID,
			Game::Net::NetworkMessage* const message,
			Resource::ReadPipe* const pipe)
		{
			if (!message->load(pipe))
			{
				status = Disconnecting;

				onThreatIdentified(
					messageID,
					L"operator invalid messagecontent",
					Device::Net::ThreatLevel::Uncommon);

				beginMessage(Host::AuthMessageID::InternalError, 8);
				sendMessage();

				return false;
			}

			return true;
		}

	private:
		void onAuthenticate(const Client::AuthenticationMessage& request)
		{
			Database::UserAuthentication user;
			const Database::ConditionResult result = Database::Interface::GetPlayerAuth(
				user,
				request.username);

			switch (result)
			{
			case Database::ConditionResult::NotFound:
				beginMessage(Host::AuthMessageID::RejectAuthentication);
				sendMessage();

				onThreatIdentified(
					Client::AuthMessageID::Authenticate,
					L"wrong username",
					Device::Net::ThreatLevel::Suspicious);

				return;
			case Database::ConditionResult::Error:
				beginMessage(Host::AuthMessageID::InternalError, 8);
				sendMessage();

				return;
			}

			char messageHash[OPERATOR_HASH_SIZE];

			Device::Encryption::HashHashSalt(
				(unsigned char*) messageHash,
				(unsigned char*) request.content.hash,
				(unsigned char*) user.salt);

			if (memcmp(messageHash, user.hash, OPERATOR_HASH_SIZE) != 0)
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

			message.userID = user.userID;

			sendCommonMessage(
				Host::AuthMessageID::AcceptAuthentication,
				&message);
			
			status = Authenticated;
			userID = user.userID;

			onClientAuthenticated();
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

			UserID userID;

			const Database::Interface::CreatePlayerResult result = Database::Interface::CreateNewPlayer(
				&userID,
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
					userID))
			{
				beginMessage(Host::AuthMessageID::InternalError, 8);
				sendMessage();

				return;
			}
			
			message.userID = userID;
			
			sendCommonMessage(
				Host::AuthMessageID::AcceptRegistration,
				&message);

			status = Authenticated;
			this->userID = userID;

			onClientAuthenticated();
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
			UserID userID;
			Database::ConditionResult result = Database::Interface::FindUserID(
				&userID,
				request.token);

			switch (result)
			{
			case Database::ConditionResult::NotFound:
				beginMessage(Host::AuthMessageID::RejectToken);
				sendMessage();

				break;
			case Database::ConditionResult::Error:
				beginMessage(Host::AuthMessageID::InternalError);
				sendMessage();

				break;
			}

			Host::AcceptTokenMessage message;
			message.userID = userID;

			sendCommonMessage(
				Host::AuthMessageID::AcceptToken,
				&message);

			status = Authenticated;
			this->userID = userID;

			onClientAuthenticated();
		}
	};
}
