#pragma once

#include <SFML/Graphics.hpp>

namespace Menu
{
	class ElementBase
	{
	public:
		ElementBase(
			ElementBase* const parent = NULL)
			:
			parent(parent)
		{
		}

		virtual void onEvent(
			const sf::Event event) = 0;
		virtual void onLogic(
			const sf::Time time) = 0;
		virtual void onDraw() = 0;

		bool isUseOnLogic() const
		{
			return useOnLogic;
		}

		bool isUseOnEvent() const
		{
			return useOnEvent;
		}

		void setSpecificOffset(
			const sf::Vector2f offset)
		{
			this->specificOffset = offset;

			loadPosition();
		}

		virtual sf::Vector2f getSize() const = 0;
		virtual sf::Vector2f getPosition() const = 0;

		virtual void loadPosition() = 0;

	protected:
		sf::Vector2f convertPosition(
			const sf::Vector2f position) const
		{
			if (parent == NULL)
			{
				return position + specificOffset;
			}
			else
			{
				return position + parent->getPosition() + specificOffset;
			}
		}

		sf::Vector2f convertPosition(
			const float x,
			const float y) const
		{
			convertPosition(
				sf::Vector2f(x, y)
			);
		}

		ElementBase* getParent() const
		{
			return parent;
		}

		bool useOnEvent = false;
		bool useOnLogic = false;

	private:
		sf::Vector2f specificOffset;

		ElementBase* parent = NULL;
	};
}
