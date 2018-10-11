#pragma once

#include <Client/source/device/ScreenDevice.h>
#include <SFML/Graphics.hpp>

#ifndef SCENE
#define SCENE ::Scene
#endif

namespace Scene
{
	enum class Type
	{
		Boot,
		Win,
		SFML
	};

	class Base
	{
	public:
		Base(Type type)
			:
			type(type)
		{
		}

		virtual bool onCreate() = 0;
		virtual bool onRemove() = 0;

		virtual void onShow() = 0;
		virtual void onHide() = 0;

		virtual void onEvent(
			sf::Event event) = 0;
		virtual void onLogic(
			sf::Time time) = 0;
		virtual void onDraw() = 0;

		Type getType() const
		{
			return type;
		}
	private:
		Type type;
	};
}
