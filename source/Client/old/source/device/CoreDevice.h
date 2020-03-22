#pragma once

#include <SFML/Graphics.hpp>

#ifndef DEVICE
#define DEVICE ::Device // remove THIS
#endif
 
namespace Device::Core
{
		enum class Error
		{
			Success,

			InputError,
			ResourceError,
			ScreenError,
			SettingsError,
			NetError
		};
		
		Error Initialize(); // think about other return [LATER]
		void Uninitialize();
		int RunGameLoop();
}
