#pragma once

#define OPERATOR_KEY_SIZE 17
#define OPERATOR_HASH_SIZE 20
#define OPERATOR_SALT_SIZE 16
#define OPERATOR_USER_SIZE 20

namespace Operator
{
	struct AuthenticationToken
	{
		unsigned char token[20];
	};

	typedef sf::Uint64 UserID;
}
