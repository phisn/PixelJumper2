#pragma once

#include "NetCore/RequestHandler.h"
#include "NetCore/message/OperatorRequestMessage.h"

#include <unordered_map>

namespace Net
{
	struct RequestInterface
	{
		enum class Reason
		{
			Success,
			Timeout,
			Failure
		};

		virtual void onRequestFinished(const Reason reason) = 0;

		// true if request can be removed
		virtual bool onMessage(
			const MessageID messageID,
			Resource::ReadPipe* const pipe) = 0;
	};

	class OperatorRequestHandler
		:
		public RequestHandler
	{
		struct RequestWrapper
		{
			~RequestWrapper()
			{
				if (message.payload)
					delete message.payload;
			}

			sf::Time age;

			OperatorRequestMessage message;
			RequestInterface* request;
		};

	public:
		struct Settings
		{
			sf::Time updateInterval;
			sf::Time requestTimeout;
		};

		OperatorRequestHandler(const Settings settings)
			:
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
					request->request->onRequestFinished(RequestInterface::Reason::Timeout);
					requests.erase(request);
				}
				else
				{
					++request;
				}
			}
		}

		bool onMessage(
			const MessageID messageID, 
			Resource::ReadPipe* const pipe) override
		{
			if (messageID == Host::OperatorRequestMessageID::OperatorRequest)
			{
				OperatorRequestMessage message;

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
							request->request->onRequestFinished(RequestInterface::Reason::Success);
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
				request.request->onRequestFinished(RequestInterface::Reason::Failure);
			}

			requests.clear();
		}

		void resendAllRequests()
		{
			for (RequestWrapper& request : requests)
			{
				access->sendMessage(
					Client::OperatorRequestMessageID::OperatorRequest,
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
		const Settings settings;

		std::vector<RequestWrapper> requests;
	};
}
