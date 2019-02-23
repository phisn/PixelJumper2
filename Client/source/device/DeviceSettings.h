#pragma once

#include <Client/source/device/ScreenDevice.h>
#include <Client/source/device/InputDevice.h>

namespace Device
{


	// replace for ResourceDevice
	namespace Settings
	{
		bool Initialize();
		void Uninitialize();

		bool LoadScreenSettings(
			ScreenSettings* const settings);
		bool SaveScreenSettings(
			ScreenSettings* const settings);

		bool LoadGeneralInputSettings(
			GeneralInputSettings* const settings);
		bool SaveGeneralInputSettings(
			GeneralInputSettings* const settings);

		bool LoadGameInputSettings(
			GameInputSettings* const settings,
			const int playerNumber);
		bool SaveGameInputSettings(
			GameInputSettings* const settings,
			const int playerNumber);
	}
}
