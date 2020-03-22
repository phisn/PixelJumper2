#pragma once

#include <SFML/Graphics.hpp>

#ifndef SCENE
#define SCENE ::Scene
#endif

namespace Scene
{
	class MainSceneBase
	{
	public:
		virtual ~MainSceneBase() { }

		_Success_(return == true)
		// can fail & must NOT call (framework) interface
		virtual bool onCreate() = 0;
		virtual void onRemove() = 0;

		// cant fail & can use Interface
		virtual void initialize() = 0;

		virtual void onShow() = 0;
		virtual void onHide() = 0;
		
		virtual void onScenePopped(const int size)
		{
		}

		virtual void onEvent(const sf::Event event) = 0;
		virtual void onLogic(const sf::Time time) = 0;
		virtual void onDraw(sf::RenderTarget* const target) = 0;
	};
}
