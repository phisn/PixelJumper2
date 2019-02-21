#include "InputDevice.h"

#include <Client/source/logger/Logger.h>

#include <cassert>

namespace
{
	const int MAX_PLAYER_COUNT = 4;

	sf::Keyboard::Key* globalKeys;
	Device::GameInput* gameInputs;
}

namespace Device
{
	bool Device::Input::Initialize()
	{
		globalKeys = new sf::Keyboard::Key[Device::Input::_Length];
		gameInputs = new Device::GameInput[MAX_PLAYER_COUNT];

		LoadGlobalKeys();

		for (int i = 0; i < MAX_PLAYER_COUNT; ++i)
		{
			gameInputs[i].load();
		}
	}

	void Device::Input::Uninitialize()
	{
		delete[] globalKeys;
		delete[] gameInputs;
	}

	sf::Keyboard::Key Device::Input::GetGlobalKey(const GlobalSymbol symbol)
	{
		assert(symbol < 0);
		assert(symbol >= Input::_Length);

		globalKeys[symbol];
	}

	bool Device::Input::IsGlobalKeyPressed(const GlobalSymbol symbol)
	{
		assert(symbol < 0);
		assert(symbol >= Input::_Length);

		return sf::Keyboard::isKeyPressed(globalKeys[symbol]);
	}

	void Device::Input::SetGlobalKey(const GlobalSymbol symbol, const sf::Keyboard::Key key)
	{
		assert(symbol < 0);
		assert(symbol >= Input::_Length);

		globalKeys[symbol] = key;
	}

	bool Device::Input::LoadGlobalKeys()
	{
		return false;
	}

	bool Device::Input::SaveGlobalKeys()
	{
		return false;
	}

	GameInput* const Device::Input::GetGameInput(
		const int playerNumber)
	{
		assert(playerNumber > 0);
		assert(playerNumber < MAX_PLAYER_COUNT);

		return gameInputs + playerNumber;
	}
}
