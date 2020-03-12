#pragma once

#include "RequestContainer.h"

#include "Resource/pipes/MemoryPipe.h"

namespace Net
{
	class ClientHandler
		:
		public RequestContainer
	{
	public:
		ClientHandler(const HSteamNetConnection connection)
			:
			connection(connection),
			networkInterface(SteamNetworkingSockets())
		{
		}

		virtual bool processMessages()
		{
			while (true)
			{
				ISteamNetworkingMessage* message = NULL;
				const int count = networkInterface->ReceiveMessagesOnConnection(
					connection,
					&message,
					1);

				if (count == 0)
				{
					return true;
				}

				if (count < 0)
				{
					Log::Error(L"Got invalid connection in clienthandler receive");
					return false;
				}

				Resource::MemoryReadPipe pipe(
					(const char*)message->m_pData,
					message->m_cbSize);

				if (MessageID messageID; pipe.readValue(&messageID))
				{
					onMessage(messageID, &pipe);
				}
				else
				{
					onThreatIdentified(
						-1,
						L"invalid message content hard",
						ThreatLevel::Suspicious);
				}

				message->Release();
			}
		}

		HSteamNetConnection getConnection() const
		{
			return connection;
		}

	protected:
		HSteamNetConnection connection;

		// message can be null to send messageid
		// without content
		// does not allow reserve anymore 
		// because pipe remains at highest send
		// capacity
		// sendmessage returns bool if sending
		// fails but calls onMessageSendFailed
		// additionally
		virtual bool sendMessage(
			const MessageID messageID,
			NetworkMessage* const message = NULL,
			const int flags = k_nSteamNetworkingSend_Reliable) override
		{
			// guranteed to succeed
			messageSendPipe.writeValue(&messageID);

			if (message && !message->save(&messageSendPipe))
			{
				messageSendPipe.reset();
				onMessageSendFailed(
					messageID,
					SendFailure::Save);

				return false;
			}

			if (messageSendPipe.getSize() > k_cbMaxSteamNetworkingSocketsMessageSizeSend)
			{
				messageSendPipe.reset();
				Log::Error(L"Tried to send too big message (over 524kb)");
				onMessageSendFailed(
					messageID,
					SendFailure::Size);

				return false;
			}

			const EResult result = networkInterface->SendMessageToConnection(
				connection,
				messageSendPipe.getRawData(),
				(uint32)messageSendPipe.getSize(),
				flags,
				NULL);

			messageSendPipe.reset();
			if (result != EResult::k_EResultOK)
			{
				onMessageSendFailed(
					messageID,
					SendFailure::Send);

				return false;
			}

			return true;
		}

		virtual bool sendMessage(
			const MessageID messageID,
			NetworkMessage* const message = NULL,
			const int flags = k_nSteamNetworkingSend_Reliable) override
		{
			// guranteed to succeed
			messageSendPipe.writeValue(&messageID);

			if (message && !message->save(&messageSendPipe))
			{
				messageSendPipe.reset();
				onMessageSendFailed(
					messageID,
					SendFailure::Save);

				return false;
			}

			if (messageSendPipe.getSize() > k_cbMaxSteamNetworkingSocketsMessageSizeSend)
			{
				messageSendPipe.reset();
				Log::Error(L"Tried to send too big message (over 524kb)");
				onMessageSendFailed(
					messageID,
					SendFailure::Size);

				return false;
			}

			const EResult result = networkInterface->SendMessageToConnection(
				connection,
				messageSendPipe.getRawData(),
				(uint32)messageSendPipe.getSize(),
				flags,
				NULL);

			messageSendPipe.reset();
			if (result != EResult::k_EResultOK)
			{
				onMessageSendFailed(
					messageID,
					SendFailure::Send);

				return false;
			}

			return true;
		}

		virtual void onMessage(
			const MessageID messageID,
			Resource::ReadPipe* const pipe)
		{
			if (!callHandlersOnMessage(messageID, pipe))
			{
				onMessageUnused(messageID, pipe);
			}
		}

		virtual void onMessageUnused(
			const MessageID messageID,
			Resource::ReadPipe* const pipe)
		{
			onThreatIdentified(
				messageID,
				L"invalid messageid",
				ThreatLevel::Suspicious);
		}

		virtual void onMessageSendFailed(
			const MessageID messageID,
			const SendFailure failure) = 0;

	protected:
		ISteamNetworkingSockets* const networkInterface;

	private:
		Resource::MemoryWritePipe messageSendPipe;
	};
}
