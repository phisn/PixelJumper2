#pragma once

#include <SFML/Graphics.hpp>

namespace Editor
{
	enum class ConnectionSide
	{
		Top,
		Left,
		Bottom,
		Right
	};

	struct ClassicContextConnectionElement
	{
		virtual void notifyBoundsChanged() = 0;
		virtual sf::FloatRect getGlobalBounds() const = 0;
	};

	class ClassicContextConnection
	{
	public:
		ClassicContextConnection()
		{
		}

		ClassicContextConnection(
			ClassicContextConnectionElement* sourceElement,
			ClassicContextConnectionElement* targetElement)
			:
			sourceElement(sourceElement),
			targetElement(targetElement)
		{
		}

		virtual void setEndpointPosition(
			ClassicContextConnectionElement* world,
			ConnectionSide side,
			sf::Vector2f position) = 0;

		virtual void setEndpointOut(
			ClassicContextConnectionElement* world,
			bool out) = 0;

		ClassicContextConnectionElement* getSourceElement()
		{
			return sourceElement;
		}

		ClassicContextConnectionElement* getTargetElement()
		{
			return targetElement;
		}

	protected:
		ClassicContextConnectionElement* sourceElement;
		ClassicContextConnectionElement* targetElement;
	};
}