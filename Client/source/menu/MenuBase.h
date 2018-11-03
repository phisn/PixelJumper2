#pragma once

#include <SFML/Graphics.hpp>

#include <functional>

namespace Menu
{
	class Base
	{
	public:
		virtual ~Base() { }

		virtual void onEvent(
			const sf::Event event) = 0;
		virtual void onLogic(
			const sf::Time time) = 0;
		virtual void onDraw() = 0;

		bool isUseOnLogic() const
		{
			return useOnLogic;
		}

		bool isUseOnEvent() const
		{
			return useOnLogic;
		}
	protected:
		bool useOnEvent = false;
		bool useOnLogic = false;
	};
}