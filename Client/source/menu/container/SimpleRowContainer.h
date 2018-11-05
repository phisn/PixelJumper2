#pragma once

#include <Client/source/menu/container/ContainerBase.h>

#include <vector>

namespace Menu
{
	class SimpleRowContainer
		:
		public ContainerBase
	{
	public:
		enum class Direction
		{
			Horizontal,
			Vertical
		};

		struct Style
		{
			float elementMargin;
			sf::Vector2f
				size,
				position;
		};

		SimpleRowContainer(
			ElementBase* const parent,
			const Style style,
			const Direction direction)
			:
			ContainerBase(parent),
			style(style),
			direction(direction)
		{
		}
		
		virtual ~SimpleRowContainer() { }

		void initialize(
			Properties* const properties) override
		{

		}
		virtual void onEvent(
			const sf::Event event)
		{
			for (ElementBase* const element : elements)
				element->onEvent(event);
		}

		virtual void onLogic(
			const sf::Time time)
		{
			for (ElementBase* const element : elements)
				if (element->isUseOnLogic())
				{
					element->onLogic(time);
				}
		}

		virtual void onDraw()
		{
			for (ElementBase* const element : elements)
				if (element->isUseOnEvent())
				{
					element->onDraw();
				}
		}

		sf::Vector2f getSize() const override
		{
			return style.size;
		}

		sf::Vector2f getPosition() const override
		{
			return position;
		}

		void loadPosition() override
		{
			position = convertPosition(style.position);

			for (ElementBase* const element : elements)
			{
				element->loadPosition();
			}
		}

		virtual void addElement(
			ElementBase* const element) override
		{
			if (elements.size() > 0)
			{
				ElementBase* const lastElement = elements.back();

				if (direction == Direction::Horizontal)
				{
					element->setSpecificOffset(sf::Vector2f(
						lastElement->getPosition().x + lastElement->getSize().x + style.elementMargin,
						lastElement->getPosition().y
					));
				}
				else
				{
					element->setSpecificOffset(sf::Vector2f(
						lastElement->getPosition().x,
						lastElement->getPosition().y + lastElement->getSize().y + style.elementMargin
					));
				}
			}
			else
			{
				element->setSpecificOffset(sf::Vector2f(
					style.elementMargin,
					style.elementMargin));
			}

			elements.push_back(element);
		}

		virtual void clearElements() override
		{
			elements.clear();
		}

	private:
		const Style style;
		const Direction direction;

		sf::Vector2f position;

		std::vector<ElementBase*> elements;
	};
}

