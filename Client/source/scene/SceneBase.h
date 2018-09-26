#pragma once

#include <Client/source/device/WindowDevice.h>
#include <SFML/Graphics.hpp>

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
		virtual bool onCreate() = 0;
		virtual bool onRemove() = 0;

		virtual void onShow() = 0;
		virtual void onHide() = 0;

		virtual void onEvent(
			sf::Event event) = 0;
		virtual void onLogic(
			sf::Time time) = 0;
		virtual void onDraw(
			::Device::Window* window) = 0;

		Type getType() const
		{
			return type;
		}
	private:
		Type type;
	};
}
