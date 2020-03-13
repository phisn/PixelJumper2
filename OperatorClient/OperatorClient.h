#pragma once

#include "NetCore/Client.h"
#include "NetCore/message/OperatorRequestMessage.h"

#include "Resource/pipes/MemoryPipe.h"

#include <unordered_map>

namespace Net
{
	struct OperatorRequest
	{
		virtual void onMessage(
			const MessageID messageID,
			Resource::ReadPipe* const pipe) = 0;
	};

	class OperatorClient
		:
		public Client
	{
		OperatorClient(const SteamNetworkingIPAddr address)
			:
			address(address)
		{
		}

		static OperatorClient* client;

		static sf::Time processInterval;
		static sf::Time processCounter;

	public:
		static void Initialize(
			const SteamNetworkingIPAddr address,
			const sf::Time interval = sf::milliseconds(100))
		{
			processInterval = interval;

			if (client)
				delete client;

			client = new OperatorClient(address);
		}

		static void Process()
		{
			client->processMessages();
		}

	private:
		const SteamNetworkingIPAddr address;

		std::unordered_map<RequestID, OperatorRequest*> requests;

		void processMessages()
		{
			while (true)
			{
				ISteamNetworkingMessage* message = NULL;
				const int count = getNetworkInterface()->ReceiveMessagesOnConnection(
					getSocket(),
					&message,
					1);

				if (count == 0)
				{
					break;
				}

				if (count < 0)
				{
					Log::Error(L"process messages in operatorclient failed");
					break;
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
					Log::Error(L"Got invalid message size",
						message->m_cbSize, L"size");
				}

				message->Release();
			}
		}

		void onMessage(
			const MessageID messageID,
			Resource::ReadPipe* const pipe)
		{
			switch (messageID)
			{
			case CommonMessageID::InternalError:

				break;
			case CommonMessageID::ExternalError:

				break;
			case Host::OperatorRequestMessageID::OperatorRequest:
				if (OperatorRequestMessage message; message.load(pipe))
				{
					onOperatorRequest(message, pipe);
				}

				break;
			default:

				break;
			}
		}

		void onInternalError()
		{
		}

		void onExternalError()
		{
		}

		void onOperatorRequest(
			const OperatorRequestMessage& message,
			Resource::ReadPipe* const pipe)
		{
			decltype(requests)::iterator request = requests.find(message.content.requestID);

			if (request == requests.end())
			{
				Log::Error(L"got unkown requestid");
			}
			else
			{
				request->second->onMessage(
					message.content.messageID, 
					pipe);
			}
		}

		void onConnectionOpened() override
		{

		}

		void onConnectionLost(const int reason) override
		{

		}

		void onConnectionClosed(const int reason) override
		{

		}
	};
}
