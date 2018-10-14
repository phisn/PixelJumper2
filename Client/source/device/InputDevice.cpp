#include "InputDevice.h"

#include <Client/source/device/DeviceInterface.h>

namespace Device
{
	bool GlobalInput::initialize()
	{
		// load from file

		settings = defaultGlobalSettings;

		return true;
	}

	LocalInput GlobalInput::loadLocalInput(
		const int position) const
	{
		// load from file

		switch (position)
		{
		case 1:
			return LocalInput(localInputSettings1);
		}
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

			Device::Interface::getInput()->saveLocalInput(
				position, this);
		}
	}
}
