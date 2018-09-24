#pragma once

#include <SFML/Graphics.hpp>

namespace Scene
{
	class Base
	{
	public:
		virtual bool onCreate() = 0;
		virtual bool onRemove() = 0;

		virtual void onShow() = 0;
		virtual void onHide() = 0;

		virtual void onEvent(
			sf::Event event) = 0;
		virtual void onLogic(
			sf::Time time) = 0;
		virtual void onDraw() = 0;
	};
}
