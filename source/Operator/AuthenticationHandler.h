#pragma once

#include "ActiveUserContainer.h"
#include "database/DatabaseInterface.h"
#include "database/UserTable.h"

#include "Common/Common.h"
#include "Common/EncryptionModule.h"
#include "Common/RandomModule.h"
#include "NetCore/message/OperatorAuthenticationMessage.h"
#include "NetCore/RequestHandler.h"

#include "database/UnlockedWorldsTable.h"

namespace Operator
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
			const Database::ConditionResult result = GetPlayerAuth(
				user,
				request.username);

			switch (result)
			{
			case Database::ConditionResult::NotFound:
				sendAuthenticationFailure(::Net::Host::AuthenticationFailureReason::AuthenticationRejected);

				access->onThreatIdentified(
					::Net::Client::OperatorAuthenticationMessageID::Authenticate,
					L"wrong username",
					::Net::ThreatLevel::Uncommon);

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
				access->onThreatIdentified(
					::Net::Client::OperatorAuthenticationMessageID::Authenticate,
					L"wrong password",
					::Net::ThreatLevel::Uncommon);

				sendAuthenticationFailure(::Net::Host::AuthenticationFailureReason::AuthenticationRejected);
				return;
			}

			switch (GetUserMode(user.userID))
			{
			case Usermode::Hosting:
				sendAuthenticationFailure(::Net::Host::AuthenticationFailureReason::AlreadyHosting);
				
				return;
			case Usermode::Online:
				sendAuthenticationFailure(::Net::Host::AuthenticationFailureReason::AlreadyOnline);

				return;
			}

			::Net::Host::AcceptOperatorAuthenticationMessage message;

			if (!CreatePlayerToken(
					message.content.authenticationToken,
					user.userID))
			{
				access->sendMessage(
					::Net::CommonMessageID::InternalError,
					NULL);

				return;
			}

			message.content.userID = user.userID;

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
			const CreatePlayerResult result = CreateNewPlayer(
				&message.content.userID,
				message.content.authenticationToken,
				salt,
				hash,
				request.username,
				request.content.key);

			switch (result)
			{
			case CreatePlayerResult::UsernameUsed:
				RejectRegistration(::Net::Host::RejectOperatorRegistrationMessageContent::UsernameUsed);

				return;
			case CreatePlayerResult::KeyUsed:
				RejectRegistration(::Net::Host::RejectOperatorRegistrationMessageContent::KeyUsed);

				access->onThreatIdentified(
					::Net::Client::OperatorAuthenticationMessageID::Authenticate,
					L"used key",
					::Net::ThreatLevel::Suspicious);

				return;
			case CreatePlayerResult::KeyNotFound:
				RejectRegistration(::Net::Host::RejectOperatorRegistrationMessageContent::KeyInvalid);

				access->onThreatIdentified(
					::Net::Client::OperatorAuthenticationMessageID::Authenticate,
					L"invalid key",
					::Net::ThreatLevel::Uncommon);

				return;
			case CreatePlayerResult::Error:
				access->sendMessage(
					::Net::CommonMessageID::InternalError,
					NULL);

				return;
			}

			access->sendMessage(
				::Net::Host::OperatorAuthenticationMessageID::AcceptRegistration,
				&message);

			callback->onAuthenticated(message.content.userID);
		}

		void RejectRegistration(const ::Net::Host::RejectOperatorRegistrationMessageContent::Reason reason)
		{
			::Net::Host::RejectOperatorRegistrationMessage message;
			message.content.reason = reason;

			access->sendMessage(
				::Net::Host::OperatorAuthenticationMessageID::RejectRegistration,
				&message);
		}

		void onTokenAuthentication(const ::Net::Client::OperatorTokenMessage& request)
		{
			UserID userID;
			Database::ConditionResult result = FindUserID(
				&userID,
				request.content.token);

			if (GetUserMode(userID) == Usermode::Online)
			{
				access->onThreatIdentified(
					::Net::Client::OperatorAuthenticationMessageID::Authenticate,
					L"token already online",
					::Net::ThreatLevel::Uncommon);

				sendAuthenticationFailure(::Net::Host::AuthenticationFailureReason::AlreadyOnline);
				return;
			}

			switch (result)
			{
			case Database::ConditionResult::NotFound:
				sendAuthenticationFailure(::Net::Host::AuthenticationFailureReason::AuthenticationRejected);

				access->onThreatIdentified(
					::Net::Client::OperatorAuthenticationMessageID::Token,
					L"invalid token",
					::Net::ThreatLevel::Uncommon);

				return;
			case Database::ConditionResult::Error:
				access->sendMessage(
					::Net::CommonMessageID::InternalError,
					NULL);

				return;
			}

			::Net::Host::AcceptOperatorTokenMessage message;
			message.content.userID = userID;

			access->sendMessage(
				::Net::Host::OperatorAuthenticationMessageID::AcceptToken,
				&message);

			callback->onAuthenticated(userID);
		}

		void sendAuthenticationFailure(::Net::Host::AuthenticationFailureReason reason)
		{
			::Net::Host::AuthenticationFailureMessage message;
			message.content.reason = ::Net::Host::AuthenticationFailureReason::AuthenticationRejected;

			access->sendMessage(
				::Net::Host::OperatorAuthenticationMessageID::AuthenticationFailure,
				&message);
		}
	};
}
