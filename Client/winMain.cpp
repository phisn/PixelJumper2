#include <iostream>

#include <Client/source/device/CoreDevice.h>
#include <Client/source/logger/Logger.h>

bool loginOperator();

#if defined(_DEBUG) || !defined(_WIN32)
	int main()
#else
	#include <Windows.h>
	int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
#endif
{
#ifdef _DEBUG
	Log::Output::Add(Log::Output::CONSOLE_OUT, Log::Level::Information);
#else
	Log::Output::Add(Log::Output::FILE_OUT, Log::Level::Warning);
#endif
	const Device::Core::Error error = Device::Core::Initialize();

	if (error != Device::Core::Error::Success)
	{
		std::cin.ignore();
		return (int) error;
	}

	if (!loginOperator())
	{
		std::cin.ignore();
		std::cin.ignore();
		return -1;
	}
	
	Log::Information(L"Starting game");
	int result = Device::Core::RunGameLoop();
	Device::Core::Uninitialize();

	std::cin.ignore();
	std::cin.ignore();
	return result;
}

#include <Client/source/operator/OperatorConnectionHandler.h>
#include <Client/source/operator/request/AuthenticationRequest.h>

#include <chrono>
#include <thread>

void awaitOperator(bool& waiting)
{
	sf::Clock clock;
	
	Log::Information(L"waiting for operator");

	while (waiting)
	{
		Operator::ConnectionHandler::Process(clock.restart());

		std::this_thread::sleep_for(
			std::chrono::milliseconds(20)
		);
	}
}

bool loginAuthenticate(std::string username, std::string password)
{
	char hash[32];
	Device::Encryption::HashCommon(
		(unsigned char*) hash,
		(unsigned char*) password.c_str(),
		password.size());

	Operator::Net::Client::AuthenticationMessage* message =
		new Operator::Net::Client::AuthenticationMessage();

	memcpy(&message->content.hash,
		hash,
		OPERATOR_HASH_SIZE);

	message->username = username;

	bool waiting = true;
	bool success = false;

	Operator::AuthenticationRequest* request = new Operator::CommonAuthenticationRequest(
		[&waiting, &success](const Operator::UserID user)
		{
			waiting = false;
			success = true;
			Log::Information(L"accepted", user, L"user");
		},
		[&waiting]()
		{
			waiting = false;
			Log::Information(L"rejected");
		},
		[&waiting](const Operator::Request::Reason reason)
		{
			waiting = false;
			Log::Information(L"failed", (int)reason, L"reason");
		});

	if (!Operator::ConnectionHandler::PushRequest(
			Operator::Net::Client::AuthMessageID::Authenticate,
			message,
			request))
	{
		Log::Information(L"failed to push request");
	}

	awaitOperator(waiting);
	return success;
}

bool loginRegister(std::string username, std::string password, std::string key)
{
	char hash[32];
	Device::Encryption::HashCommon(
		(unsigned char*)hash,
		(unsigned char*)password.c_str(),
		password.size());

	Operator::Net::Client::RegistrationMessage* message =
		new Operator::Net::Client::RegistrationMessage();

	memcpy(&message->content.hash,
		hash,
		OPERATOR_HASH_SIZE);

	memcpy(&message->content.key,
		key.c_str(),
		OPERATOR_KEY_SIZE);

	message->username = username;

	bool waiting = true;
	bool success = false;

	Operator::CommonRegistrationRequest* request = new Operator::CommonRegistrationRequest(
		[&waiting, &success](const Operator::UserID user)
		{
			waiting = false;
			success = true;
			Log::Information(L"accepted", user, L"user");
		},
		[&waiting](const Operator::Net::Host::RejectRegistrationMessage::Reason reason)
		{
			waiting = false;
			Log::Information(L"rejected", (int)reason, L"reason");
		},
			[&waiting](const Operator::Request::Reason reason)
		{
			waiting = false;
			Log::Information(L"failed", (int)reason, L"reason");
		});

	if (!Operator::ConnectionHandler::PushRequest(
			Operator::Net::Client::AuthMessageID::Register,
			message,
			request))
	{
		Log::Information(L"failed to push request");
	}

	awaitOperator(waiting);
	return success;
}

bool loginOperator()
{
	SteamNetworkingIPAddr ipAddress;
//	ipAddress.ParseString("109.230.236.76:9928");
	ipAddress.SetIPv6LocalHost(9928);

	Operator::ConnectionHandler::Initialize(ipAddress);

	int option;

	do
	{
		std::cout << "Options:" << std::endl;
		std::cout << "1. Register" << std::endl;
		std::cout << "2. Authenticate" << std::endl;
		std::cout << "> ";

		std::cin >> option;
	} 
	while (option != 1 && option != 2);

	std::string password, username, key;

	std::cout << "username: ";
	std::cin >> username;

	if (username.length() < 4 || username.length() > 16)
	{
		std::cout << "invalid username length" << std::endl;
		return false;
	}

	std::cout << "password: ";
	std::cin >> password;

	if (password.length() < 4 || password.length() > 16)
	{
		std::cout << "invalid username length" << std::endl;
		return false;
	}

	if (option == 1)
	{
		std::cout << "format: xxxxx-xxxxx-xxxxx" << std::endl;
		std::cout << "key:";
	
		std::string rawKey;
		std::cin >> rawKey;

		if (rawKey.length() != 17)
		{
			std::cout << "invalid key length" << std::endl;
			return false;
		}

		for (int i = 0; i < 17; ++i)
			if (rawKey[i] != '-')
				key.push_back(rawKey[i]);

		if (key.length() != 15)
		{
			std::cout << "invalid key content" << std::endl;
			return false;
		}
	}

	if (option == 2)
	{
		return loginAuthenticate(username, password);
	}
	else
	{
		return loginRegister(username, password, key);
	}
}
