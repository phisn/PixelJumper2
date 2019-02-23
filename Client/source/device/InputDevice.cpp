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
		gameInputs = (Device::GameInput*) new char[MAX_PLAYER_COUNT * sizeof(Device::GameInput)];

		LoadGlobalKeys();

		for (int i = 0; i < MAX_PLAYER_COUNT; ++i)
		{
			new(&gameInputs[i]) Device::GameInput(i);
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

	int Input::GetKeyUsageCount(const sf::Keyboard::Key key)
	{
		int usageCount = 0;

		for (int i = 0; i < GlobalSymbol::_Length; ++i)
			if (globalKeys[i] == key)
			{
				++usageCount;
			}

		for (int i = 0; i < MAX_PLAYER_COUNT; ++i)
		{
			for (int j = 0; j < (int) GameCoreInputSymbol::_Length; ++i)
				if (gameInputs[i].getCoreKey((GameCoreInputSymbol) j) == key)
				{
					++usageCount;
				}

			for (int j = 0; j < (int) GameViewInputSymbol::_Length; ++i)
				if (gameInputs[i].getViewKey((GameViewInputSymbol) j) == key)
				{
					++usageCount;
				}
		}

		return usageCount;
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
