#pragma once

#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>

namespace Scene
{
	class SubBase
	{
	public:
		virtual bool onCreate() = 0;
		virtual void onRemove() = 0;

		virtual void onEvent(
			sf::Event event) = 0;
		virtual void onLogic(
			sf::Time time) = 0;
		virtual void onDraw() = 0;
	};
}
