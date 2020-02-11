#pragma once

#include <Client/source/device/NetDevice.h>
#include <Client/source/operator/OperatorAccess.h>

#include <Operator/source/net/AuthClientMessage.h>
#include <Operator/source/net/AuthOperatorMessage.h>

namespace Operator
{
	class Authenticator
		:
		public Device::Net::Client,
		public AuthenticatorAccess
	{
	public:
		bool AuthenticateCredentials(
			const char hash[OPERATOR_HASH_SIZE],
			const std::string username) override
		{
			Operator::Net::Client::AuthenticationMessage message;

			memcpy(message.content.hash,
				hash,
				OPERATOR_HASH_SIZE);

			message.username = username;

			if (!sendCommonMessage(Operator::Net::Client::AuthMessageID::Authenticate, &message))
			{
				return false;
			}

			status = Authenticating;

			return true;
		}

		bool AuthenticateToken(const char token[OPERATOR_HASH_SIZE]) override
		{
			memcpy(this->token.token,
				token,
				OPERATOR_HASH_SIZE);


		}

	private:
		AuthenticationToken token;
		Operator::UserID userID;

		void onMessage(
			const Device::Net::MessageID messageID,
			Resource::ReadPipe* const pipe) override
		{
			switch (messageID)
			{
			case Operator::Net::Host::AuthMessageID::AcceptRegistration:
			{
				Operator::Net::Host::AcceptRegistrationMessage message;

				if (!message.load(pipe))
				{
					onThreatIdentified(
						messageID,
						L"invalid messagecontent",
						Device::Net::ThreatLevel::Uncommon);

					return;
				}

				onAcceptRegistration(message);
			}
				break;
			case Operator::Net::Host::AuthMessageID::RejectRegistration:
			{
				Operator::Net::Host::RejectRegistrationMessage message;

				if (!message.load(pipe))
				{
					onThreatIdentified(
						messageID,
						L"invalid messagecontent",
						Device::Net::ThreatLevel::Uncommon);

					return;
				}

				onRejectRegistration(message);
			}
				break;
			case Operator::Net::Host::AuthMessageID::AcceptToken:
			{
				Operator::Net::Host::AcceptTokenMessage message;

				if (!message.load(pipe))
				{
					onThreatIdentified(
						messageID,
						L"invalid messagecontent",
						Device::Net::ThreatLevel::Uncommon);

					return;
				}

				onAcceptToken(message);
			}
				break;
			case Operator::Net::Host::AuthMessageID::RejectToken:
				onRejectToken();

				break;
			case Operator::Net::Host::AuthMessageID::AcceptAuthentication:
			{
				Operator::Net::Host::AcceptAuthenticationMessage message;

				if (!message.load(pipe))
				{
					onThreatIdentified(
						messageID,
						L"invalid messagecontent",
						Device::Net::ThreatLevel::Uncommon);

					return;
				}

				onAcceptAuthentication(message);
			}
				break;
			case Operator::Net::Host::AuthMessageID::RejectAuthentication:
			{
				onRejectAuthentication();
			}
				break;
			case Operator::Net::Host::AuthMessageID::Timeout:
			{
				onTimeout();
			}
				break;
			case Operator::Net::Host::AuthMessageID::InternalError:
			{
				onInternalError();
			}
				break;
			}
		}

		void onConnectionOpen() override
		{
			// push event
		}

		void onConnectionLost(const int reason) override
		{
			status = Unconnected;

			// push event
		}

		void onConnectionClosed(const int reason) override
		{
		}

		void onThreatIdentified(
			const Device::Net::MessageID messageID,
			const wchar_t* const note,
			const Device::Net::ThreatLevel level) override
		{
		}

		void onAcceptRegistration(const Operator::Net::Host::AcceptRegistrationMessage& message)
		{
			memcpy(
				token.token,
				message.authenticationToken,
				OPERATOR_HASH_SIZE);

			userID = message.userID;

			// push event

			status = Status::Connected;
		}

		void onRejectRegistration(const Operator::Net::Host::RejectRegistrationMessage& message)
		{
			message.reason;

			// push event

			status = Status::Unconnected;
		}

		void onAcceptAuthentication(const Operator::Net::Host::AcceptAuthenticationMessage& message)
		{
			memcpy(
				token.token,
				message.authenticationToken,
				OPERATOR_HASH_SIZE);

			userID = message.userID;

			// push event

			status = Status::Connected;
		}

		void onRejectAuthentication()
		{
			status = Status::Unconnected;
		}

		void onAcceptToken(const Operator::Net::Host::AcceptTokenMessage& message)
		{
			userID = message.userID;

			// push event

			status = Status::Connected;
		}

		void onRejectToken()
		{
			// push event

			status = Status::Unconnected;
		}

		void onTimeout()
		{
			// push event

			status = Status::Unconnected;
		}

		void onInternalError()
		{
			// push event

			status = Status::Unconnected;
		}

		bool sendCommonMessage(
			Device::Net::MessageID messageID,
			Game::Net::NetworkMessage* const message)
		{
			if (!message->save(beginMessage(messageID)) || !sendMessage())
			{
				Log::Error(L"Unable to construct message",
					(sf::Uint64) messageID, L"messageID");

				return false;
			}
			
			return true;
		}
	};
}
