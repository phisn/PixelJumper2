#pragma once

#include <Client/source/device/NetDevice.h>

#include <Client/source/game/net/UserAuthClientMessage.h>
#include <Client/source/game/net/UserAuthHostMessage.h>

#include <Client/source/operator/OperatorConnectionHandler.h>

namespace Game::Net
{
	// authhandler as base for remoteconnection
	// should not be run without authentication
	class HostAuthenticationHandler
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

		HostAuthenticationHandler(const sf::Uint32 timeout = 100)
			:
			timeout(timeout)
		{
		}

		// has to override and only be called
		// when status == Connecting
		virtual void update()
		{
			if (status == Connecting && --timeout == 0)
			{
				status = Disconnecting;

				beginMessage(Host::AuthenticationMessageID::Timeout, 8);
				sendMessage();

				return;
			}

			if (!process())
			{
				status = Disconnecting;

				// think about removing this because
				// it has a very high chance to fail
				// (gurranted?)
				beginMessage(Host::AuthenticationMessageID::InternalError, 8);
				sendMessage();

				return;
			}
		}

		Status getStatus() const
		{
			return status;
		}

	protected:
		using ClientHandler::process;

		Status status = Connecting;
		Resource::PlayerID userID = NULL;

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
			{
				Client::AuthenticationMessage message;

				if (loadCommonMessage(messageID, &message, pipe))
				{
					return;
				}

				onAuthenticate(message);
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

		void sendSimpleMessage(Device::Net::MessageID messageID)
		{
			if (beginMessage(messageID) || !sendMessage())
			{
				Log::Error(L"Unable to send message",
					(sf::Uint64) messageID, L"messageID");

				status = Disconnecting;
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

				beginMessage(Host::AuthenticationMessageID::InternalError, 8);
				sendMessage();

				return false;
			}

			return true;
		}

	private:
		void onAuthenticate(const Client::AuthenticationMessage& request)
		{
			Operator::ConnectionKeySource keySource;

			memcpy(keySource.token.token,
				Operator::ConnectionHandler::GetToken().token,
				OPERATOR_HASH_SIZE);
			keySource.userID = request.userID;

			if (request.key.validate(keySource))
			{
				sendSimpleMessage(Host::AuthenticationMessageID::AuthenticationAccepted);

				status = Connected;
				userID = request.userID;

				onClientConnected();
			}
			else
			{
				Host::AuthenticationRejectedMessage message;
				message.reason = Host::AuthenticationRejectedMessageContent::Reason::InvalidConnectionKey;

				sendCommonMessage(
					Host::AuthenticationMessageID::AuthenticationRejected,
					&message);
			}
		}
	};
}
