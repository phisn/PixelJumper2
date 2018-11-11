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
		typedef Properties Properties;

		enum class Direction
		{
			Horizontal,
			Vertical
		};

		struct Style
		{
			float elementMargin;
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
			this->properties = *(Properties*) properties;
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
			return properties.size;
		}

		sf::Vector2f getPosition() const override
		{
			return position;
		}

		void loadPosition() override
		{
			position = convertPosition(properties.position);

			for (ElementBase* const element : elements)
			{
				element->loadPosition();
			}
		}

		virtual void addElement(
			ElementBase* const element) override
		{
			element->setParent(this);

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

			useOnEvent |= element->isUseOnEvent();
			useOnLogic |= element->isUseOnLogic();
		}

		virtual void clearElements() override
		{
			elements.clear();

			useOnEvent = false;
			useOnLogic = false;
		}

		virtual bool isEmpty() const override
		{
			return elements.empty();
		}

	private:
		Properties properties;
		const Style style;
		const Direction direction;

		sf::Vector2f position;

		std::vector<ElementBase*> elements;
	};
}

