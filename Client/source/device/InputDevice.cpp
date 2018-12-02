#include "InputDevice.h"

#include <Client/source/device/DeviceInterface.h>
#include <Client/source/logger/Logger.h>

#include <Windows.h>

namespace
{
	DEVICE::LocalInput* inputs[4] = { };
}

namespace Device
{
	bool GlobalInput::initialize()
	{
		Log::Section section(L"Initializing InputDevice");

		if (inputs[0])
		{ // Quicker than loop
			delete inputs[0];
			delete inputs[1];
			// delete inputs[2];
			// delete inputs[3];
		}

		// load from file
		settings = defaultGlobalSettings;

		// load from file
		inputs[0] = new LocalInput(localInputSettings1);
		inputs[1] = new LocalInput(localInputSettings2);

		return true;
	}

	LocalInput* GlobalInput::loadLocalInput(
		const int position) const
	{
		if (position >= 0 && position <= 4)
		{
			return inputs[position];
		}

		MessageBoxW(
			NULL,
			L"Attempted to load invalid input slot",
			L"Error",
			MB_OK);

		return NULL;
	}

	void GlobalInput::saveLocalInput(
		const int position, 
		const LocalInput* input) const
	{
	}

	void GlobalInput::saveSettings() const
	{
		if (changed)
		{
			changed = false;

			// ...
		}
	}

	GlobalInputSymbol GlobalInput::codeToSymbol(
		const sf::Keyboard::Key key)
	{
		static_assert((int)GlobalInputSymbol::_Length > 1, L"Invalid GlobalInputSymbol Elements");

		for (int i = 0; i < (int)GlobalInputSymbol::_Length; ++i)
			if (settings.keys[i] == key)
			{
				return (GlobalInputSymbol)i;
			}

		return GlobalInputSymbol::Invalid;
	}

	void Device::LocalInput::saveSettings(
		const int position) const
	{
		if (changed)
		{
			changed = false;

			Device::Interface::GetInput()->saveLocalInput(
				position, this);
		}
	}
}
