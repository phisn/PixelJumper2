#pragma once

#include <Operator/source/operator/AuthenticationHandler.h>

namespace Operator::Net
{
	class RequestHandler
		:
		public AuthenticationHandler
	{
	public:
		AuthenticationHandler::AuthenticationHandler;

		void onThreatIdentified(
			const Device::Net::MessageID messageID,
			const wchar_t* const note,
			const Device::Net::ThreatLevel level) override
		{
		}
		
		void onClientAuthenticated() override
		{
			Log::Information(L"Client Authenticated");
		}
	};
}
