#pragma once

#include <SFML/Graphics.hpp>

#ifndef DEVICE
#define DEVICE ::Device
#endif

namespace Device
{
	/*

		[Device]Settings classes provided for config generation

	*/

	class GlobalInput;
	class Screen;
	class Resource;

	enum class InitError
	{
		Invalid = -1,

		Input,
		Network,
		Scene,
		Screen,
		Resource,

		_Length
	};

	namespace Interface
	{
		InitError Initialize();
		int Start();

		GlobalInput* GetInput();
		Screen* GetScreen();
		Resource* GetResource();
	}
}
