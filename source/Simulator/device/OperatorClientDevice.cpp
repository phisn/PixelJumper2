#include "OperatorClientDevice.h"
#include "OperatorClient/OperatorClient.h"
#include "OperatorClient/request/CommonAuthenticationRequest.h"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace Device::OperatorClient
{
	bool Initialize(std::string token_file_name)
	{
		if (!std::filesystem::exists(token_file_name))
		{
			Log::Error(L"token file does not exist, create a new one with \"token\" as parameter");
			return false;
		}

		char token[OPERATOR_HASH_SIZE];
		std::ifstream token_file{ token_file_name };
		token_file.read(token, OPERATOR_HASH_SIZE);
		token_file.close();

		if (!token_file)
		{
			Log::Error(L"failed to read token file, create a new one with \"token\" as parameter");
			return false;
		}

		Net::Client::OperatorTokenMessage* message =
			new Net::Client::OperatorTokenMessage;
		Operator::CommonAuthenticationRequest request;

		memcpy(message->token, token, OPERATOR_HASH_SIZE);

		Operator::Client::PushRequestFailure result = Operator::Client::PushRequest(
			Net::Client::OperatorAuthenticationMessageID::Authenticate,
			message,
			&request);

		if (result != Operator::Client::PushRequestFailure::Success)
		{
			Log::Error(L"push request failed", (int) result, L"reason");
			return false;
		}

		return Operator::AwaitSyncRequest(&request);
	}

	void AquireToken(std::string token_file_name)
	{
		std::string password;
		Net::Client::OperatorAuthenticationMessage* message =
			new Net::Client::OperatorAuthenticationMessage;

		std::cout << "username: ";
		std::cin >> message->username;

		std::cout << "password: ";
		std::cin >> password;

		Module::Encryption::HashCommon(
			(unsigned char*) message->content.hash,
			(unsigned char*) password.c_str(),
			password.length());

		Operator::CommonAuthenticationRequest request;

		const Operator::Client::PushRequestFailure result = Operator::Client::PushRequest(
			Net::Client::OperatorAuthenticationMessageID::Authenticate,
			message,
			&request);

		if (result != Operator::Client::PushRequestFailure::Success)
		{
			Log::Error(L"push request failed", (int) result, L"reason");
			return;
		}

		if (!Operator::AwaitSyncRequest(&request))
		{
			Log::Error(L"authentication request failed");
			return;
		}

		std::cout << "token successfully" << std::endl;

		std::ofstream token_file{ token_file_name };
		token_file.write(
			(const char*) Operator::Client::GetToken().token, 
			OPERATOR_HASH_SIZE);
		token_file.close();

		if (token_file)
		{
			std::cout << "token successfully save" << std::endl;
		}
		else
		{
			std::cout << "failed to save token" << std::endl;
		}
	}
}
