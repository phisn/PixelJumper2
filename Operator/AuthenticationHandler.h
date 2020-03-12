#pragma once

#include "database/DatabaseInterface.h"

#include "Common/Common.h"
#include "Common/EncryptionModule.h"
#include "Common/RandomModule.h"
#include "NetCore/message/OperatorAuthenticationMessage.h"
#include "NetCore/RequestHandler.h"

namespace Operator::Net
{
	struct AuthenticationHandlerCallback
	{
		virtual void onAuthenticated(const UserID userID) = 0;
		virtual void onAuthenticationDenied() = 0;
	};

	class AuthenticationHandler
		:
		public ::Net::RequestHandler
	{
	public:
		AuthenticationHandler(
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
				access->sendMessage(
					Host::OperatorAuthenticationMessageID::Timeout,
					NULL);

				callback->onAuthenticationDenied();
			}
		}

		bool onMessage(
			const ::Net::MessageID messageID,
			Resource::ReadPipe* const pipe) override
		{
			Log::Information(L"got message", messageID, L"messageID");

			switch (messageID)
			{
			case ::Net::Client::OperatorAuthenticationMessageID::Authenticate:
				if (::Net::Client::AuthenticationMessage message; loadMessage(messageID, &message, pipe))
				{
					onAuthenticate(message);
				}

				return true;

			case ::Net::Client::OperatorAuthenticationMessageID::Register:
				if (::Net::Client::RegistrationMessage message; loadMessage(messageID, &message, pipe))
				{
					onRegistration(message);
				}

				return true;

			case ::Net::Client::OperatorAuthenticationMessageID::Token:
				if (::Net::Client::TokenMessage message; loadMessage(messageID, &message, pipe))
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

		void onAuthenticate(const ::Net::Client::AuthenticationMessage& request)
		{
			UserAuthentication user;
			const Database::ConditionResult result = DatabaseInterface::GetPlayerAuth(
				user,
				request.username);

			switch (result)
			{
			case Database::ConditionResult::NotFound:
				access->sendMessage(
					Host::OperatorAuthenticationMessageID::RejectAuthentication,
					NULL);

				access->onThreatIdentified(
					::Net::Client::OperatorAuthenticationMessageID::Authenticate,
					L"wrong username",
					::Net::ThreatLevel::Suspicious);

				return;
			case Database::ConditionResult::Error:
				access->sendMessage(
					::Net::CommonMessageID::InternalError,
					NULL);

				return;
			}

			char messageHash[OPERATOR_HASH_SIZE];

			Module::Encryption::HashHashSalt(
				(unsigned char*)messageHash,
				(unsigned char*)request.content.hash,
				(unsigned char*)user.salt);

			if (memcmp(messageHash, user.hash, OPERATOR_HASH_SIZE) != 0)
			{
				access->sendMessage(
					Host::OperatorAuthenticationMessageID::RejectAuthentication,
					NULL);

				return;
			}

			Host::AcceptAuthenticationMessage message;

			if (!DatabaseInterface::CreatePlayerToken(
				message.authenticationToken,
				user.userID))
			{
				access->sendMessage(
					::Net::CommonMessageID::InternalError,
					NULL);

				return;
			}

			message.userID = user.userID;

			access->sendMessage(
				Host::OperatorAuthenticationMessageID::AcceptAuthentication,
				&message);

			callback->onAuthenticated(user.userID);
		}

		void onRegistration(const ::Net::Client::RegistrationMessage& request)
		{
			char hash[OPERATOR_HASH_SIZE];
			char salt[OPERATOR_SALT_SIZE];

			Module::Random::FillRandom(OPERATOR_SALT_SIZE, salt);

			Module::Encryption::HashHashSalt(
				(unsigned char*)hash,
				(unsigned char*)request.content.hash,
				(unsigned char*)salt);

			Host::AcceptRegistrationMessage message;
			const DatabaseInterface::CreatePlayerResult result = DatabaseInterface::CreateNewPlayer(
				&message.userID,
				message.authenticationToken,
				salt,
				hash,
				request.username,
				request.content.key);

			switch (result)
			{
			case DatabaseInterface::CreatePlayerResult::UsernameUsed:
				RejectRegistration(Host::RejectRegistrationMessage::UsernameUsed);

				return;
			case DatabaseInterface::CreatePlayerResult::KeyUsed:
				RejectRegistration(Host::RejectRegistrationMessage::KeyUsed);

				access->onThreatIdentified(
					::Net::Client::OperatorAuthenticationMessageID::Authenticate,
					L"used key",
					::Net::ThreatLevel::Suspicious);

				return;
			case DatabaseInterface::CreatePlayerResult::KeyNotFound:
				RejectRegistration(Host::RejectRegistrationMessage::KeyInvalid);

				access->onThreatIdentified(
					::Net::Client::OperatorAuthenticationMessageID::Authenticate,
					L"invalid key",
					::Net::ThreatLevel::Uncommon);

				return;
			case DatabaseInterface::CreatePlayerResult::Error:
				access->sendMessage(
					::Net::CommonMessageID::InternalError,
					NULL);

				return;
			}

			access->sendMessage(
				Host::OperatorAuthenticationMessageID::AcceptRegistration,
				&message);

			callback->onAuthenticated(message.userID);
		}

		void RejectRegistration(const Host::RejectRegistrationMessage::Reason reason)
		{
			Host::RejectRegistrationMessage message;
			message.reason = reason;

			access->sendMessage(
				Host::OperatorAuthenticationMessageID::RejectRegistration,
				&message);
		}

		void onTokenAuthentication(const ::Net::Client::TokenMessage& request)
		{
			UserID userID;
			Database::ConditionResult result = DatabaseInterface::FindUserID(
				&userID,
				request.token);

			switch (result)
			{
			case Database::ConditionResult::NotFound:
				access->sendMessage(
					Host::OperatorAuthenticationMessageID::RejectToken,
					NULL);

				break;
			case Database::ConditionResult::Error:
				access->sendMessage(
					::Net::CommonMessageID::InternalError,
					NULL);

				break;
			}

			Host::AcceptTokenMessage message;
			message.userID = userID;

			access->sendMessage(
				Host::OperatorAuthenticationMessageID::AcceptToken,
				&message);

			callback->onAuthenticated(userID);
		}
	};
}
