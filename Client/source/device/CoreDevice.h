#pragma once

#include <SFML/Graphics.hpp>

#ifndef DEVICE
#define DEVICE ::Device // remove THIS
#endif

namespace Device
{	
	namespace Core
	{
		enum class Error
		{
			Success,

			InputError,
			ResourceError,
			ScreenError
		};
		
		Error Initialize(); // think about other return [LATER]
		int RunGameLoop();
	}

	namespace Interface
	{
		InitError Initialize();
		int Start();

		GlobalInput* GetInput();
		Screen* GetScreen();
		Resource* GetResource();
	}
}
