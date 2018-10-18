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
			const sf::Event event) = 0;
		virtual void onLogic(
			const sf::Time time) = 0;
		virtual void onDraw() = 0;
	};
}
