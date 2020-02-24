#pragma once

#include <Client/source/device/NetDevice.h>
#include <Client/source/device/EncryptionDevice.h>

#include <Operator/source/database/DatabaseInterface.h>
#include <Operator/source/net/AuthClientMessage.h>
#include <Operator/source/net/AuthOperatorMessage.h>

#include <Client/source/net/RequestHandlerBase.h>

namespace Operator::Net
{
	struct AuthenticationHandlerCallback
	{
		virtual void onAuthenticated(const Operator::UserID userID) = 0;
		virtual void onAuthenticationDenied() = 0;
	};

	/*
		registration can currently fail but create the player
		without the player knowing the creation. when the creation
		of a token fails at a registration 
	*/
	class _AuthenticationHandler
		:
		public ::Net::RequestHandler
	{
	public:
		_AuthenticationHandler(
			AuthenticationHandlerCallback* const callback,
			const sf::Uint32 timeout)
			:
			callback(callback),
			timeout(timeout)
		{
		}

		void update() override
		{
			if (--timeout == 0)
			{
				access->accessSendMessage(
					Host::AuthMessageID::Timeout,
					NULL);

				callback->onAuthenticationDenied();
			}
		}
		
		bool onMessage(
			const Device::Net::MessageID messageID,
			Resource::ReadPipe* const pipe) override
		{
			Log::Information(L"got message", messageID, L"messageID");

			switch (messageID)
			{
			case Client::AuthMessageID::Authenticate:
				if (Client::AuthenticationMessage message; loadMessage(messageID, &message, pipe))
				{
					onAuthenticate(message);
				}

				return true;

			case Client::AuthMessageID::Register:
				if (Client::RegistrationMessage message; loadMessage(messageID, &message, pipe))
				{
					onRegistration(message);
				}

				return true;

			case Client::AuthMessageID::Token:
				if (Client::TokenMessage message; loadMessage(messageID, &message, pipe))
				{
					onTokenAuthentication(message);
				}

				return true;
			}

			return false;
		}

	private:
		AuthenticationHandlerCallback* const callback;

		sf::Uint32 timeout;

		void onAuthenticate(const Client::AuthenticationMessage& request)
		{
			Database::UserAuthentication user;
			const Database::ConditionResult result = Database::Interface::GetPlayerAuth(
				user,
				request.username);

			switch (result)
			{
			case Database::ConditionResult::NotFound:
				access->accessSendMessage(
					Host::AuthMessageID::RejectAuthentication,
					NULL);
				
				access->accessOnThreatIdentified(
					Client::AuthMessageID::Authenticate,
					L"wrong username",
					Device::Net::ThreatLevel::Suspicious);

				return;
			case Database::ConditionResult::Error:
				access->accessSendMessage(
					Host::AuthMessageID::InternalError,
					NULL);

				return;
			}

			char messageHash[OPERATOR_HASH_SIZE];

			Device::Encryption::HashHashSalt(
				(unsigned char*) messageHash,
				(unsigned char*) request.content.hash,
				(unsigned char*) user.salt);

			if (memcmp(messageHash, user.hash, OPERATOR_HASH_SIZE) != 0)
			{
				access->accessSendMessage(
					Host::AuthMessageID::RejectAuthentication,
					NULL);

				return;
			}

			Host::AcceptAuthenticationMessage message;

			if (!Database::Interface::CreatePlayerToken(
				message.authenticationToken,
				user.userID))
			{
				access->accessSendMessage(
					Host::AuthMessageID::InternalError,
					NULL);

				return;
			}

			message.userID = user.userID;

			access->accessSendMessage(
				Host::AuthMessageID::AcceptAuthentication,
				&message);

			callback->onAuthenticated(user.userID);
		}

		void onRegistration(const Client::RegistrationMessage& request)
		{
			char hash[OPERATOR_HASH_SIZE];
			char salt[OPERATOR_SALT_SIZE];

			Device::Random::FillRandom(OPERATOR_SALT_SIZE, salt);

			Device::Encryption::HashHashSalt(
				(unsigned char*)hash,
				(unsigned char*)request.content.hash,
				(unsigned char*)salt);

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
				access->accessSendMessage(
					Host::AuthMessageID::InternalError,
					NULL);

				return;
			}

			Host::AcceptRegistrationMessage message;

			if (!Database::Interface::CreatePlayerToken(
				message.authenticationToken,
				userID))
			{
				access->accessSendMessage(
					Host::AuthMessageID::InternalError,
					NULL);

				return;
			}

			message.userID = userID;

			access->accessSendMessage(
				Host::AuthMessageID::AcceptRegistration,
				&message);

			callback->onAuthenticated(userID);
		}

		void RejectRegistration(const Host::RejectRegistrationMessage::Reason reason)
		{
			Host::RejectRegistrationMessage message;
			message.reason = reason;

			access->accessSendMessage(
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
				access->accessSendMessage(
					Host::AuthMessageID::RejectToken,
					NULL);

				break;
			case Database::ConditionResult::Error:
				access->accessSendMessage(
					Host::AuthMessageID::InternalError,
					NULL);

				break;
			}

			Host::AcceptTokenMessage message;
			message.userID = userID;

			access->accessSendMessage(
				Host::AuthMessageID::AcceptToken,
				&message);

			callback->onAuthenticated(userID);
		}
	};
}
