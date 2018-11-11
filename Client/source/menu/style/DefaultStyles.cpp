#include "DefaultStyles.h"

namespace Menu
{
	namespace DefaultStyle
	{
		ScrollBar::Style ScrollBar::style =
		{
			// colors
			sf::Color::Color(50, 50, 50), // background_fill
			sf::Color::Color(0, 0, 0), // background_outline

			sf::Color::Color(100, 100, 100), // default_fill
			sf::Color::Color(0, 0, 0), // default_outline

			sf::Color::Color(150, 150, 150), // enter_fill
			sf::Color::Color(0, 0, 0), // enter_outline

			sf::Color::Color(200, 200, 200), // click_fill
			sf::Color::Color(0, 0, 0), // click_outline

			// outline thickness
			0.0f, // background
			0.0f, // default
			0.0f, // enter
			0.0f, // click
		};
	}
}
