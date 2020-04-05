#pragma once

#include "NetCore.h"
#include "RequestHandler.h"

#include <typeindex>

namespace Net
{
	class RequestContainer
	{
	public:
		virtual ~RequestContainer()
		{
			// deleting all remaining requesthandlers that are normally not
			// stored in clienthandlers
			for (RequestHandler* const requestHandler : requestHandlers)
				delete requestHandler;
		}

	protected:
		virtual ConnectionAccess* getConnectionAccess() = 0;

		template <typename T>
		// value has to be created with new and wont be
		// available after remove has been called
		void addRequestHandler(T* const value)
		{
			requestHandlers.push_back(value);
			value->initialize(getConnectionAccess());
			requestHandlerTypes.push_back(typeid(T));
		}

		template <typename T, typename... Args>
		void createRequestHandler(Args&&... arguments)
		{
			addRequestHandler(new T(std::forward(arguments)...));
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
					T* const value = (T*)requestHandlers[i];

					requestHandlers.erase(requestHandlers.begin() + i);
					requestHandlerTypes.erase(requestHandlerTypes.begin() + i);

					return value;
				}

			return NULL;
		}

		bool callHandlersOnMessage(
			const Net::MessageID messageID,
			Resource::ReadPipe* const pipe) const
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
