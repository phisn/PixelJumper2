#pragma once

#include <SFML/System.hpp>

namespace Device
{
	enum class InputSymbol
	{
		Invalid,

		OpenMenu,
		Pause,
		Exit
	};

	class Input
	{
	public:
		InputSymbol codeToSymbol(
			sf::Keyboard::Key key)
		{

		}
	};
}
