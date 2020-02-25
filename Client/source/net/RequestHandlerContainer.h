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
		

	private:
		std::vector<RequestHandler*> requestHandlers;
		std::vector<std::type_index> requestHandlerTypes;
	};
}
