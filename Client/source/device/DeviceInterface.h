#pragma once

#include <SFML/Graphics.hpp>

#ifndef DEVICE
#define DEVICE ::Device // remove THIS
#endif

namespace Device
{	
	namespace Interface
	{
		bool Initialize(); // think about other return [LATER]
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
