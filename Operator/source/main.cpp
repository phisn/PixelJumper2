#include <Client/source/logger/Logger.h>
#include <Client/source/device/RandomDevice.h>

#include <Operator/source/device/CoreDevice.h>

#include <Operator/source/database/DatabaseInterface.h>

#include <iostream>

// important
// a player is not different to a server
// both can host a game or join a game
// a server differs from a player that it does not
// contain graphics code
// both have to login and authenticate

int main()
{
	Log::Output::Add(Log::Output::CONSOLE_OUT, Log::Level::Warning);

	if (Device::Core::Initialize())
	{
		/*for (int i = 0; i < 10; ++i)
		{
			Database::Interface::CreateNewKey(NULL);
		}*/

		Resource::PlayerID player;
		char hash[20];
		char salt[16];
		Device::Random::FillRandom(20, hash);
		Device::Random::FillRandom(16, salt);

		std::string username = "user1";
		Database::RegistrationKey key;
		memcpy(key.content,
			"ASCTGZGMURUKQYD",
			OPERATOR_KEY_SIZE);

		Database::Interface::CreatePlayerResult result = Database::Interface::CreateNewPlayer(
			&player,
			salt,
			hash,
			username,
			key);

		if (result == Database::Interface::CreatePlayerResult::Success)
		{
			std::cout << "Create new player: " << player << std::endl;
		}
		else
		{
			Log::Error(L"Failed to create new player",
				(int) result, L"result");
		}

		Device::Core::Run();
		Device::Core::Uninitialize();

		return 0;
	}
	else
	{
		return 1;
	}
}
