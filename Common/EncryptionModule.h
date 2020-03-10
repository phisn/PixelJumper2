#pragma once

namespace Module::Encryption
{
	void HashCommon(
		unsigned char result[32],
		const unsigned char* buffer,
		const size_t length);
	void HashHashSalt(
		unsigned char result[32],
		const unsigned char hash[32],
		const unsigned char salt[16]);
}
