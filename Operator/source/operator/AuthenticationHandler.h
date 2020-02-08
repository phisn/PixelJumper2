#pragma once

#include <Client/source/device/NetDevice.h>

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
			case Client::AuthMessageID::Authenticate:
				Client::AuthenticationMessage message;

				if (!message.load(pipe))
				{
					this->onThreatIdentified(
						messageID,
						L"invalid messagecontent",
						Device::Net::ThreatLevel::Uncommon);

					beginMessage(Host::, 8);
					sendMessage();

					return;
				}

				// authentication

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

		void safeMessageLoad(
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
		
	};
}
