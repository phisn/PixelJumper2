#pragma once

#define OPERATOR_KEY_SIZE 15
#define OPERATOR_HASH_SIZE 20
#define OPERATOR_SALT_SIZE 16
#define OPERATOR_USER_SIZE 20

namespace Operator
{
	struct AuthenticationToken
	{
		unsigned char token[20];
	};

	// xxxxx-xxxxx-xxxxx
	struct RegistrationKey
	{
		char content[15];
	};

	typedef sf::Uint64 UserID;
}
