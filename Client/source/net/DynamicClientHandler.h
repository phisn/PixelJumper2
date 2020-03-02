#pragma once

#include <Client/source/device/NetDevice.h>
#include <Client/source/net/DynamicRequestContainer.h>

namespace Net
{
	class DynamicClientHandler
		:
		public Device::Net::ClientHandler,
		public DynamicRequestContainer
	{
	public:
		DynamicClientHandler(const HSteamNetConnection connection)
			:
			ClientHandler(connection)
		{
		}

	protected:
		virtual void onMessage(
			const Device::Net::MessageID messageID,
			Resource::ReadPipe* const pipe) override
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
			Game::Net::NetworkMessage* const message = NULL) override
		{
			// message can be null
			if (message)
			{
				if (!message->save(beginMessage(messageID)))
				{
					onMessageSendFailed(messageID, SendFailure::Save);
					return false;
				}
			}
			else
			{
				beginMessage(messageID, 8);
			}

			if (!ClientHandler::sendMessage())
			{
				onMessageSendFailed(messageID, SendFailure::Send);
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
