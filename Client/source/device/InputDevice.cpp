#include "InputDevice.h"

#include <Client/source/device/DeviceConfig.h>
#include <Client/source/logger/Logger.h>

#include <cassert>

namespace
{
	Device::GeneralInputSettings* generalInput;
	Device::GameInput* gameInputs;
}

namespace Device
{
	bool Input::Initialize()
	{
		generalInput = new GeneralInputSettings();
		gameInputs = (GameInput*) new char[(int) Player::_Length * sizeof(GameInput)];

		LoadGlobalKeys();

		for (int i = 0; i < (int) Player::_Length; ++i)
		{
			new(&gameInputs[i]) GameInput((Player) i);

			if (!gameInputs[i].load())
			{
				return false;
			}
		}

		return true;
 	}

	void Input::Uninitialize()
	{
		delete generalInput;
		delete[] gameInputs;
	}

	sf::Keyboard::Key Input::GetGlobalKey(const GlobalSymbol symbol)
	{
		assert(symbol < 0);
		assert(symbol >= Input::_Length);

		return generalInput->GlobalKeys.keys[(int) symbol];
	}

	bool Input::IsGlobalKeyPressed(const GlobalSymbol symbol)
	{
		assert(symbol < 0);
		assert(symbol >= Input::_Length);

		return sf::Keyboard::isKeyPressed(generalInput->GlobalKeys.keys[symbol]);
	}

	void Input::SetGlobalKey(const GlobalSymbol symbol, const sf::Keyboard::Key key)
	{
		assert(symbol < 0);
		assert(symbol >= Input::_Length);

		generalInput->GlobalKeys.keys[symbol] = key;
	}

	int Input::GetKeyUsageCount(const sf::Keyboard::Key key)
	{
		int usageCount = 0;

		for (int i = 0; i < GlobalSymbol::_Length; ++i)
			if (generalInput->GlobalKeys.keys[i] == key)
			{
				++usageCount;
			}

		for (int i = 0; i < (int) Player::_Length; ++i)
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

	bool Input::LoadGlobalKeys()
	{
		return Config::LoadType(
			generalInput, 
			Device::ConfigType::Input);
	}

	bool Input::SaveGlobalKeys()
	{
		return Config::SaveType(
			generalInput,
			Device::ConfigType::Input);
	}

	GameInput* const Input::GetGameInput(
		const Input::Player player)
	{
		return &gameInputs[(int) player];
	}
	
	bool GameInput::load()
	{
		return Config::LoadPlayerInput(this, player);
	}
	inline bool GameInput::save()
	{
		return Config::SavePlayerInput(this, player);
	}
}
