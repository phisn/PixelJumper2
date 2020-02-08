#include "EncryptionDevice.h"

#include <openssl/sha.h>

#include <cstring>

void Device::Encryption::HashCommon(
	unsigned char result[20],
	const unsigned char* buffer,
	const size_t length)
{
	SHA256(buffer, length, result);
}

void Device::Encryption::HashHashSalt(
	unsigned char result[20], 
	const unsigned char hash[20], 
	const unsigned char salt[16])
{
	unsigned char input[36];

	memcpy(input, hash, 20);
	memcpy(input + 20, salt, 16);

	HashCommon(result, input, 36);
}
