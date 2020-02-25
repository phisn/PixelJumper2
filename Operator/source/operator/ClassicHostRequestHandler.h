#pragma once

#include <Client/source/net/RequestHandlerBase.h>

#include <Operator/source/Common.h>
#include <Operator/source/net/ClassicSimulatorMessage.h>

namespace Operator::Net
{
	/*
		no meed for classiccommonrequesthandler or
		similar to allow user requesting of data because
		all requests should come though classic servers
		to minimize the request offhead for operator

		add the request to retrive user data
	*/
	class ClassicHostRequestHandler
		:
		public ::Net::RequestHandler
	{
	public:
		ClassicHostRequestHandler(const UserID userID)
			:
			userID(userID)
		{
		}

		void update() override
		{
		}

		bool onMessage(
			const Device::Net::MessageID messageID,
			Resource::ReadPipe* const pipe) override
		{
			switch (messageID)
			{
			case Client::ClassicHostID::RequestClientData:
			}
		}

	private:
		const UserID userID;
	};
}
