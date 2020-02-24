#pragma once

#include <Client/source/device/NetDevice.h>
#include <Client/source/net/RequestHandlerBase.h>

#include <typeindex>
#include <vector>

namespace Net
{
	class RequestHandlerContainer
		:
		public ClientHandlerAccess
	{
	protected:
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
		std::vector<RequestHandler*> requestHandlers;
		std::vector<std::type_index> requestHandlerTypes;
	};
}
