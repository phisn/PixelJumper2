#include "OperatorAccess.h"

namespace
{
	Operator::AuthenticationToken token;
}

namespace Operator
{
	bool Initialize()
	{
		return true;
	}

	void Uninitialize()
	{
	}

	void ProcessAuthenticationHandler(AuthenticationEventHandler* const handler)
	{
	}

	void AuthenticateCredentials(
		const char hash[OPERATOR_HASH_SIZE],
		const std::string username)
	{
	}

	void AuthenticateToken(const char token[OPERATOR_HASH_SIZE])
	{
	}
}
