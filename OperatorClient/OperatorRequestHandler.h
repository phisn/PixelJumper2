#pragma once

#include "request/RequestInterface.h"

#include "Common/Common.h"
#include "NetCore/RequestHandler.h"
#include "NetCore/message/OperatorRequestMessage.h"

#include <unordered_map>

namespace Operator
{
	struct ClientRequestHandlerCallback
	{
		virtual void onAuthenticated(const UserID userID) = 0;
		virtual void onAuthenticated(
			const char token[OPERATOR_HASH_SIZE],
			const UserID userID) = 0;
	};

	class ClientRequestHandler
		:
		public ::Net::RequestHandler
	{
		friend class ClientAuthenticationRequest;

		struct RequestWrapper
		{
			~RequestWrapper()
			{
				if (message.payload)
					delete message.payload;
			}

			sf::Time age;

			::Net::OperatorRequestMessage message;
			RequestInterface* request;
		};

	public:
		struct Settings
		{
			sf::Time updateInterval;
			sf::Time requestTimeout;
		};

		ClientRequestHandler(
			ClientRequestHandlerCallback* const callback,
			const Settings settings)
			:
			callback(callback),
			settings(settings)
		{
		}

		virtual void update() override
		{
			decltype(requests)::iterator request = requests.begin();
			while (request != requests.end())
			{
				request->age += settings.updateInterval;

				if (request->age > settings.requestTimeout)
				{
					request->request->onRequestFailure(RequestInterface::Reason::Timeout);
					requests.erase(request);
				}
				else
				{
					++request;
				}
			}
		}

		bool onMessage(
			const ::Net::MessageID messageID,
			Resource::ReadPipe* const pipe) override
		{
			if (messageID == ::Net::Host::OperatorRequestMessageID::OperatorRequest)
			{
				::Net::OperatorRequestMessage message;

				if (loadMessage(messageID, &message, pipe))
				{
					decltype(requests)::iterator request = std::find_if(
						requests.begin(),
						requests.end(), 
						[&message](const RequestWrapper& request)
						{
							return request.message.content.requestID == message.content.requestID;
						});

					if (request == requests.end())
					{
						Log::Warning(L"Got invalid requestID from operator. a request probably already timed out",
							message.content.messageID, L"messageID",
							message.content.requestID, L"requestID");
					}
					else
					{
						if (request->request->onMessage(
								message.content.messageID,
								pipe))
						{
							requests.erase(request);
						}
					}
				}

				return true;
			}
			
			return false;
		}

		void removeAllRequests()
		{
			for (RequestWrapper& request : requests)
			{
				request.request->onRequestFailure(RequestInterface::Reason::Internal);
			}

			requests.clear();
		}

		void resendAllRequests()
		{
			for (RequestWrapper& request : requests)
			{
				access->sendMessage(
					::Net::Client::OperatorRequestMessageID::OperatorRequest,
					&request.message);
			}
		}

		// if requests are pending and connection
		// idled -> reconnect
		bool areRequestsPending() const
		{
			return requests.size() > 0;
		}

	private:
		ClientRequestHandlerCallback* const callback;
		const Settings settings;

		std::vector<RequestWrapper> requests;
	};
}
