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
			InputError,
			ResourceError,
			ScreenError
		};
		
		Error Initialize(); // think about other return [LATER]
		void runGameLoop();
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
