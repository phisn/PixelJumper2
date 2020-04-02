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
					::Net::Host::OperatorAuthenticationMessageID::Timeout,
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
				if (::Net::Client::OperatorAuthenticationMessage message; loadMessage(messageID, &message, pipe))
				{
					onAuthenticate(message);
				}

				return true;

			case ::Net::Client::OperatorAuthenticationMessageID::Register:
				if (::Net::Client::OperatorRegistrationMessage message; loadMessage(messageID, &message, pipe))
				{
					onRegistration(message);
				}

				return true;

			case ::Net::Client::OperatorAuthenticationMessageID::Token:
				if (::Net::Client::OperatorTokenMessage message; loadMessage(messageID, &message, pipe))
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

		void onAuthenticate(const ::Net::Client::OperatorAuthenticationMessage& request)
		{
			UserAuthentication user;
			const Database::ConditionResult result = DatabaseInterface::GetPlayerAuth(
				user,
				request.username);

			switch (result)
			{
			case Database::ConditionResult::NotFound:
				access->sendMessage(
					::Net::Host::OperatorAuthenticationMessageID::RejectAuthentication,
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
					::Net::Host::OperatorAuthenticationMessageID::RejectAuthentication,
					NULL);

				return;
			}

			::Net::Host::AcceptOperatorAuthenticationMessage message;

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
				::Net::Host::OperatorAuthenticationMessageID::AcceptAuthentication,
				&message);

			callback->onAuthenticated(user.userID);
		}

		void onRegistration(const ::Net::Client::OperatorRegistrationMessage& request)
		{
			char hash[OPERATOR_HASH_SIZE];
			char salt[OPERATOR_SALT_SIZE];

			Module::Random::FillRandom(OPERATOR_SALT_SIZE, salt);

			Module::Encryption::HashHashSalt(
				(unsigned char*)hash,
				(unsigned char*)request.content.hash,
				(unsigned char*)salt);

			::Net::Host::AcceptOperatorRegistrationMessage message;
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
				RejectRegistration(::Net::Host::RejectOperatorRegistrationMessage::UsernameUsed);

				return;
			case DatabaseInterface::CreatePlayerResult::KeyUsed:
				RejectRegistration(::Net::Host::RejectOperatorRegistrationMessage::KeyUsed);

				access->onThreatIdentified(
					::Net::Client::OperatorAuthenticationMessageID::Authenticate,
					L"used key",
					::Net::ThreatLevel::Suspicious);

				return;
			case DatabaseInterface::CreatePlayerResult::KeyNotFound:
				RejectRegistration(::Net::Host::RejectOperatorRegistrationMessage::KeyInvalid);

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
				::Net::Host::OperatorAuthenticationMessageID::AcceptRegistration,
				&message);

			callback->onAuthenticated(message.userID);
		}

		void RejectRegistration(const ::Net::Host::RejectOperatorRegistrationMessage::Reason reason)
		{
			::Net::Host::RejectOperatorRegistrationMessage message;
			message.reason = reason;

			access->sendMessage(
				::Net::Host::OperatorAuthenticationMessageID::RejectRegistration,
				&message);
		}

		void onTokenAuthentication(const ::Net::Client::OperatorTokenMessage& request)
		{
			UserID userID;
			Database::ConditionResult result = DatabaseInterface::FindUserID(
				&userID,
				request.token);

			switch (result)
			{
			case Database::ConditionResult::NotFound:
				access->sendMessage(
					::Net::Host::OperatorAuthenticationMessageID::RejectToken,
					NULL);

				break;
			case Database::ConditionResult::Error:
				access->sendMessage(
					::Net::CommonMessageID::InternalError,
					NULL);

				break;
			}

			::Net::Host::AcceptOperatorTokenMessage message;
			message.userID = userID;

			access->sendMessage(
				::Net::Host::OperatorAuthenticationMessageID::AcceptToken,
				&message);

			callback->onAuthenticated(userID);
		}
	};
}
