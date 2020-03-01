#pragma once

#include <Client/source/net/RequestHandlerBase.h>

#include <typeindex>
#include <vector>

namespace Net
{
	class DynamicRequestContainer
		:
		public ConnectionAccess
	{
	public:
		virtual ~DynamicRequestContainer()
		{
			// deleting all remaining requesthandlers that are normally not
			// stored in clienthandlers
			for (RequestHandler* const requestHandler : requestHandlers)
				delete requestHandler;
		}

	protected:
		enum class SendFailure
		{
			Save,
			Send
		};

		virtual void onMessageSendFailed(
			const Device::Net::MessageID messageID,
			const SendFailure reason) = 0;
		virtual void onInvalidMessageID(
			const Device::Net::MessageID messageID) = 0;
		virtual void onMessage(
			const Device::Net::MessageID messageID,
			Resource::ReadPipe* const pipe)
		{
			if (!callHandlersOnMessage(messageID, pipe))
			{
				accessOnThreatIdentified(
					messageID,
					L"invalid messageid",
					Device::Net::ThreatLevel::Suspicious);

				onInvalidMessageID(messageID);
			}
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
		T* removeRequestHandler()
		{
			for (int i = 0; i < requestHandlerTypes.size(); ++i)
				if (requestHandlerTypes[i] == typeid(T))
				{
					T* const value = requestHandlers[i];

					requestHandlers.erase(requestHandlers.begin() + i);
					requestHandlerTypes.erase(requestHandlerTypes.begin() + i);

					return value;
				}

			return NULL;
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
		std::vector<RequestHandler*> requestHandlers;
		std::vector<std::type_index> requestHandlerTypes;
	};
}
