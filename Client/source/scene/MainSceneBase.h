#pragma once

#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>

#ifndef SCENE
#define SCENE ::Scene
#endif

namespace Scene
{
	class MainBase
	{
	public:
		virtual ~MainBase() { }

		_Success_(return == true)
		// Can fail & !!! MUST _NOT_ !!! use Interface
		virtual bool onCreate() = 0;
		virtual void onRemove() = 0;

		// Cant fail & can use Interface
		virtual void initialize() = 0;

		virtual void onShow() = 0;
		virtual void onHide() = 0;

		virtual void onEvent(
			const sf::Event event) = 0;
		virtual void onLogic(
			const sf::Time time) = 0;
		virtual void onDraw() = 0;

		bool isRunning() const
		{
			return running;
		}
	protected:
		bool running = true;
	};
}
