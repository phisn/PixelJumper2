#pragma once

#define OPERATOR_KEY_SIZE 15
#define OPERATOR_HASH_SIZE 32
#define OPERATOR_SALT_SIZE 16
#define OPERATOR_USER_SIZE 20

namespace Operator
{
	struct AuthenticationToken
	{
		unsigned char token[OPERATOR_HASH_SIZE];
	};

	// xxxxx-xxxxx-xxxxx
	struct RegistrationKey
	{
		char content[OPERATOR_KEY_SIZE];
	};

	typedef sf::Uint64 UserID;
}
