#pragma once

#include "RequestContainer.h"

#include "Resource/pipes/MemoryPipe.h"

namespace Net
{
	struct ConnectionWrapperCallback
	{
		virtual void onMessage(
			const MessageID messageID,
			Resource::ReadPipe* const pipe) = 0;
	};

	class ConnectionWrapper
	{
	public:
		ConnectionWrapper(
			ConnectionWrapperCallback* const callback,
			const HSteamNetConnection connection)
			:
			callback(callback),
			connection(connection),
			networkInterface(SteamNetworkingSockets())
		{
		}

		bool processMessages()
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

	protected:
		// does not allow reserve anymore 
		// because pipe remains at highest send
		// capacity
		// sendmessage returns bool if sending
		// fails but calls onMessageSendFailed
		// additionally
		bool sendMessage(
			const MessageID messageID,
			NetworkMessage* const message,
			const int flags = k_nSteamNetworkingSend_Reliable) override
		{
			// guranteed to succeed
			messageSendPipe.writeValue(&messageID);

			if (message && !message->save(&messageSendPipe))
			{
				messageSendPipe.reset();
				onMessageSendFailed(
					SendFailure::Save,
					messageID);

				return false;
			}

			if (messageSendPipe.getSize() > k_cbMaxSteamNetworkingSocketsMessageSizeSend)
			{
				messageSendPipe.reset();
				Log::Error(L"Tried to send too big message (over 524kb)");
				onMessageSendFailed(
					SendFailure::Size,
					messageID);

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
					SendFailure::Send,
					messageID);

				return false;
			}

			return true;
		}

		virtual void onMessageSendFailed(
			const SendFailure failure,
			const MessageID messageID) = 0;

	private:
		ConnectionWrapperCallback* const callback;
		const HSteamNetConnection connection;
		ISteamNetworkingSockets* const networkInterface;

		Resource::MemoryWritePipe messageSendPipe;
	}

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

		bool processMessages()
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

	protected:
		// does not allow reserve anymore 
		// because pipe remains at highest send
		// capacity
		// sendmessage returns bool if sending
		// fails but calls onMessageSendFailed
		// additionally
		bool sendMessage(
			const MessageID messageID,
			NetworkMessage* const message,
			const int flags = k_nSteamNetworkingSend_Reliable) override
		{
			// guranteed to succeed
			messageSendPipe.writeValue(&messageID);

			if (message && !message->save(&messageSendPipe))
			{
				messageSendPipe.reset();
				onMessageSendFailed(
					SendFailure::Save,
					messageID);

				return false;
			}

			if (messageSendPipe.getSize() > k_cbMaxSteamNetworkingSocketsMessageSizeSend)
			{
				messageSendPipe.reset();
				Log::Error(L"Tried to send too big message (over 524kb)");
				onMessageSendFailed(
					SendFailure::Size,
					messageID);

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
					SendFailure::Send,
					messageID);

				return false;
			}

			return true;
		}

		virtual void onMessage(
			const MessageID messageID, 
			Resource::ReadPipe* const pipe) = 0;

		virtual void onMessageSendFailed(
			const SendFailure failure,
			const MessageID messageID) = 0;

	private:
		const HSteamNetConnection connection;
		ISteamNetworkingSockets* const networkInterface;

		Resource::MemoryWritePipe messageSendPipe;
	};
}
