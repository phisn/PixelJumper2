#pragma once

#include <SFML/Graphics/RenderTarget.hpp>

#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>


namespace Scene
{
	class SubBase
	{
	public:
		virtual ~SubBase() { }

		virtual bool onCreate() = 0;
		virtual void onRemove() = 0;

		virtual void onEvent(
			const sf::Event event) = 0;
		virtual void onLogic(
			const sf::Time time) = 0;
		virtual void onDraw(
			sf::RenderTarget* const target) = 0;
	};
}
