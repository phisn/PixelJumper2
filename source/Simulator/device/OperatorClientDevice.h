#pragma once

#define OPERATOR_TOKEN_FILE_NAME "token.txt"

#include <string>

namespace Device::OperatorClient
{
	// pure authentication
	// unlocking of different 
	bool Initialize(std::string token_file_name = OPERATOR_TOKEN_FILE_NAME);

	// will use stdin and should not be used
	// in console
	void AquireToken(std::string token_file_name = OPERATOR_TOKEN_FILE_NAME);
}
