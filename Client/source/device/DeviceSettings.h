#pragma once

#include <Client/source/device/ScreenDevice.h>

namespace Device
{


	// replace for ResourceDevice
	namespace Settings
	{
		bool Initialize();
		void Uninitialize();

		void LoadScreenSettings(ScreenSettings* const settings);
		void SaveScreenSettings(const ScreenSettings* const settings);


	}
}
