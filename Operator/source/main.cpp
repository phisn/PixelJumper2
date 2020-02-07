#include <Client/source/logger/Logger.h>
#include <Client/source/device/RandomDevice.h>

#include <Operator/source/device/CoreDevice.h>

#include <Operator/source/database/DatabaseInterface.h>

#include <iostream>

int main()
{
	Log::Output::Add(Log::Output::CONSOLE_OUT, Log::Level::Warning);

	if (Device::Core::Initialize())
	{
		Resource::PlayerID player;
		char hash[20];
		char salt[16];
		Device::Random::FillRandom(20, hash);
		Device::Random::FillRandom(16, salt);
		std::string username = "the user";
		std::string key = "PHQGH-UMEAY-LNLFD";

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
