#include "EncryptionDevice.h"

#include <openssl/sha.h>

#include <cstring>

void Device::Encryption::HashCommon(
	unsigned char result[32],
	const unsigned char* buffer,
	const size_t length)
{
	SHA256_CTX context;
	SHA256_Init(&context);
	SHA256_Update(&context, buffer, length);
	SHA256_Final(result, &context);
}

void Device::Encryption::HashHashSalt(
	unsigned char result[32], 
	const unsigned char hash[32], 
	const unsigned char salt[16])
{
	unsigned char input[32 + 16];

	memcpy(input, hash, 32);
	memcpy(input + 32, salt, 16);

	HashCommon(result, input, 32 + 16);
}
