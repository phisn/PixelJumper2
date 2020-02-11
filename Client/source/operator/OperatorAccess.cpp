#include "OperatorAccess.h"
#include <Client/source/operator/DefaultAuthEventHandler.h>
#include <Client/source/operator/OperatorAuthenticator.h>

namespace
{
	Operator::AuthenticationEventHandler* eventHandler = NULL;
	Operator::DefaultAuthEventHandler* defaultEventHandler;
	Operator::Authenticator* authenticator;
}

namespace Operator
{
	bool Initialize()
	{
		defaultEventHandler = new DefaultAuthEventHandler();
		authenticator = new Operator::Authenticator();

		return true;
	}

	void Uninitialize()
	{
	}

	void Process()
	{
		if (!authenticator->process())
		{
			// ...
		}
	}

	void PutAuthenticationEventHandler(AuthenticationEventHandler* const eventHandler)
	{
		::eventHandler = eventHandler;
	}

	void PopAuthenticationEventHandler()
	{
		eventHandler = NULL;
	}

	AuthenticationEventHandler* getActiveAuthenticationEventHandler()
	{
		return eventHandler ? eventHandler : defaultEventHandler;
	}
}
