#pragma once

#include <Client/source/device/NetDevice.h>
#include <Client/source/net/RequestHandlerContainer.h>

#include <typeindex>
#include <vector>

namespace Net
{
	class DynamicClientHandler
		:
		public Device::Net::ClientHandler,
		public RequestHandlerContainer
	{
	public:
		DynamicClientHandler(const HSteamNetConnection connection)
			:
			ClientHandler(connection)
		{
		}

	protected:
		enum class SendFailure
		{
			Save,
			Send
		};

		virtual void onMessageSendFailed(const SendFailure reason) = 0;
		virtual void onMessage(
			const Device::Net::MessageID messageID, 
			Resource::ReadPipe* const pipe)
		{
			if (!callHandlersOnMessage(messageID, pipe))
			{
				onThreatIdentified(
					messageID,
					L"invalid messageid",
					Device::Net::ThreatLevel::Suspicious);
			}
		}

		bool accessSendMessage(
			const Device::Net::MessageID messageID,
			Game::Net::NetworkMessage* const message) override
		{
			// message can be null
			if (message)
			{
				if (!message->save(beginMessage(messageID)))
				{
					onMessageSendFailed(SendFailure::Save);
					return false;
				}
			}
			else
			{
				beginMessage(messageID, 8);
			}

			if (!ClientHandler::sendMessage())
			{
				onMessageSendFailed(SendFailure::Send);
				return false;
			}

			return true;
		}

	private:
		void accessOnThreatIdentified(
			const Device::Net::MessageID messageID,
			const wchar_t* const note,
			const Device::Net::ThreatLevel level) override
		{
			onThreatIdentified(
				messageID, 
				note,
				level);
		}
	};
}
