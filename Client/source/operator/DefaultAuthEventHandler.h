#pragma once

#include <Client/source/operator/OperatorAccess.h>

namespace Operator
{
	class DefaultAuthEventHandler
		:
		public AuthenticationEventHandler
	{
	public:
		virtual void onConnected() override
		{
		}

		virtual void onDisconnected() override
		{
		}

		virtual void onTokenAccepted() override
		{
		}

		virtual void onAuthenticationAccepted() override
		{
		}

		virtual void onRegistrationAccepted() override
		{
		}

		virtual void onTokenRejected() override
		{
		}

		virtual void onAuthenticationRejected() override
		{
		}

		virtual void onRegistrationRejected() override
		{
		}

		virtual void onTimeout() override
		{
		}

		virtual void onInternalError() override
		{
		}
	};
}
