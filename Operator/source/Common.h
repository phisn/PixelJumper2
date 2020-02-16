#pragma once

#define OPERATOR_KEY_SIZE 15
#define OPERATOR_HASH_SIZE 32
#define OPERATOR_SALT_SIZE 16
#define OPERATOR_USER_SIZE 20

#include <Client/source/device/EncryptionDevice.h>

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

	struct ConnectionKeySource
	{
		AuthenticationToken token;
		UserID userID;
	};

	struct ConnectionKey
	{
		char content[OPERATOR_HASH_SIZE];
		
		bool validate(const ConnectionKeySource& source) const
		{
			ConnectionKey temp;
			temp.make(source);

			return memcmp(&temp.content, content, OPERATOR_HASH_SIZE) == 0;
		}

		void make(const ConnectionKeySource& source)
		{
			Device::Encryption::HashCommon(
				(unsigned char*) content,
				(unsigned char*) &source,
				sizeof(ConnectionKeySource));
		}
	};
	
	struct Hash
	{
		char content[OPERATOR_HASH_SIZE];
	};

	typedef sf::Uint64 UserID;
}
