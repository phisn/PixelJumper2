#pragma once

#include <SFML/Graphics.hpp>

#ifndef SCENE
#define SCENE ::Scene
#endif

namespace Scene
{
	enum class FallbackType
	{
		Default,
		Complete, // Until main menu
		Error,
	};

	enum Type : int
	{
		SFML,
		Windows
	};

	class Base
	{
	public:
		Base(
			const bool catching,
			const Type type)
			:
			catching(catching),
			type(type)
		{
		}

		Type getType() const
		{
			return type;
		}

		bool isCatching() const
		{
			return catching;
		}

		virtual bool onCreate() = 0;
		virtual void onRemove() = 0;

		virtual void onShow() = 0;
		virtual void onHide() = 0;
		virtual void onCatch(
			FallbackType type) // Fallback
		{
		}

		virtual void onEvent(
			sf::Event event) = 0;
		virtual void onLogic(
			sf::Time time) = 0;
		virtual void onDraw() = 0;

	private:
		bool catching; // fallback
		Type type;
	};
}