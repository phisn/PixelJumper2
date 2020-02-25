#pragma once

#include <Client/source/device/NetDevice.h>
#include <Client/source/net/RequestHandlerBase.h>

#include <typeindex>
#include <vector>

namespace Net
{
	// move to client handlerbecause other types
	// of clienthandlers are not used
	// think about creating a clienthandler with
	// the basic function of authentication as
	// needed for most (all) clienthandlers
	class DynamicClientHandler
		:
		public Device::Net::ClientHandler,
		public ClientHandlerAccess
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

		template <typename T>
		// value has to be created with new and wont be
		// available after remove has been called
		void addRequestHandler(T* const value)
		{
			requestHandlers.push_back(value);
			value->initialize(this);
			requestHandlerTypes.push_back(typeid(T));
		}

		template <typename T>
		bool existsRequestHandler()
		{
			return std::find(
				requestHandlers.begin(),
				requestHandlers.end(),
				typeid(T)
			) != requestHandlers.end();
		}

		template <typename T>
		void removeRequestHandler()
		{
			for (int i = 0; i < requestHandlerTypes.size(); ++i)
				if (requestHandlerTypes[i] == typeid(T))
				{
					delete requestHandlers[i];

					requestHandlers.erase(requestHandlers.begin() + i);
					requestHandlerTypes.erase(requestHandlerTypes.begin() + i);

					break;
				}
		}

		bool callHandlersOnMessage(
			const Device::Net::MessageID messageID,
			Resource::ReadPipe* const pipe)
		{
			for (RequestHandler* const handler : requestHandlers)
				if (handler->onMessage(messageID, pipe))
				{
					return true;
				}

			return false;
		}

		void callHandlersUpdate()
		{
			for (RequestHandler* const handler : requestHandlers)
				handler->update();
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

		std::vector<RequestHandler*> requestHandlers;
		std::vector<std::type_index> requestHandlerTypes;
	};
}
