#pragma once

#include <SFML/Graphics.hpp>

namespace Editor::ClassicContext
{
	enum class ConnectionSide
	{
		Top,
		Left,
		Bottom,
		Right
	};

	struct ConnectionElement
	{
		virtual void notifyBoundsChanged() = 0;
		virtual sf::FloatRect getGlobalBounds() const = 0;
	};

	class Connection
	{
	public:
		Connection()
		{
		}

		Connection(
			ConnectionElement* sourceElement,
			ConnectionElement* targetElement)
			:
			sourceElement(sourceElement),
			targetElement(targetElement)
		{
		}

		virtual void setEndpointPosition(
			ConnectionElement* world,
			ConnectionSide side,
			sf::Vector2f position) = 0;

		virtual void setEndpointOut(
			ConnectionElement* world,
			bool out) = 0;

		ConnectionElement* getSourceElement()
		{
			return sourceElement;
		}

		ConnectionElement* getTargetElement()
		{
			return targetElement;
		}

	protected:
		ConnectionElement* sourceElement;
		ConnectionElement* targetElement;
	};

	class DummyConnectionElement
		:
		public ConnectionElement
	{
	public:
		DummyConnectionElement(Connection* connection)
			:
			connection(connection)
		{
		}

		void notifyBoundsChanged() override
		{
		}

		sf::FloatRect getGlobalBounds() const override
		{
			return sf::FloatRect{ position.x, position.y, 0, 0 };
		}

		void setPosition(sf::Vector2f position, ConnectionSide side)
		{
			this->position = position;
			connection->setEndpointPosition(this, side, position);

			if (element)
			{
				element->notifyBoundsChanged();
			}
		}

		void setElement(ConnectionElement* element)
		{
			this->element = element;
		}

	private:
		sf::Vector2f position;

		ConnectionElement* element;
		Connection* connection;
	};
}