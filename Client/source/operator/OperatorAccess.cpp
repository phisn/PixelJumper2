#include "OperatorAccess.h"
#include <future>

namespace Operator
{
	const RequestID Operator::Request(
		RequestType request, 
		RequestHandler* const handler)
	{
		const RequestID requestID = Device::Random::MakeRandom<RequestID>();

		// simulate requests based on connection with 200ms work time
		std::async(std::launch::async, 
			[request, handler, requestID]()
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(200));

				switch (request)
				{
				case RequestType::AuthenticationToken:
					AuthenticationToken token;

					for (int i = 0; i < 20; ++i)
					{
						token.token[i] = i;
					}

					Resource::MemoryReadPipe pipe(
						(const char*) &token,
						sizeof(token));

					handler->handle(
						RequestHandler::Done,
						requestID,
						RequestType::AuthenticationToken,
						&pipe);

					break;
				case RequestType::PlayerData:


					break;
				}
			});

		return requestID;
	}
}
