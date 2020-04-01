#pragma once

#include "NetCore/RequestHandler.h"

namespace Game::Net
{
	class HostClassicCommonHandler
		:
		public ::Net::RequestHandler
	{
	public:
		void update() override
		{
		}

		bool onMessage(
			const ::Net::MessageID messageID, 
			Resource::ReadPipe* const pipe) override
		{
			switch (messageID)
			{

			}

			return false;
		}
	};
}
