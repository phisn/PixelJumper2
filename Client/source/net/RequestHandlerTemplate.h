#pragma once

#include <Client/source/net/RequestHandlerBase.h>

namespace Game::Net
{
	class ___RequetHandlerTemplate
		:
		public ::Net::RequestHandler
	{
	public:
		void update() override
		{
		}

		bool onMessage(
			const Device::Net::MessageID messageID,
			Resource::ReadPipe* const pipe) override
		{
			switch (messageID)
			{

			}

			return false;
		}
	};
}
