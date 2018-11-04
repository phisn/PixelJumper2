#pragma once

#include <Client/source/menu/MenuBase.h>

namespace Menu
{
	class ElementBase
		:
		public Base
	{
	public:
		virtual sf::Vector2f getSize() const = 0;
		virtual sf::Vector2f getPosition() const = 0;
	};
}
