#pragma once

#include <Client/source/menu/MenuCommon.h>
#include <Client/source/menu/MenuElementBase.h>

#include <cassert>
#include <functional>

namespace Menu
{
	class RowContainer
		:
		public ElementBase
	{
	public:
		RowContainer(const CommonControlDirection direction)
			:
			direction(direction)
		{
			size.addListener(
				[this](const sf::Vector2f oldSize,
					   const sf::Vector2f newSize)
				{
					//  change area
				});
		}

		void addElement(
			ElementBase* const element,
			const size_t position)
		{
			const Container& container = getChildren();
			assert(position <= container.size());

			if (container.size() > 0)
			{
				updateSingleElementPosition(
					container.back(),
					element);

				Container::const_iterator iterator = container.begin() + position;

				if (iterator == container.cend())
				{
					addChild(element);
				}
				else
				{
					insertChild(iterator, element);
				}
			}
			else
			{
				updateSingleElementPosition(element);
				addChild(element);
			}

			element->size.addListener(elementSizeListener);
			updateSizeByElementSize(element->size);
		}

		void removeElement(
			const size_t position)
		{
			removeElement(getChildren().begin() + position);
		}

		void removeElement(
			const ElementBase* const element)
		{
			Container::const_iterator iterator = std::find(
				getChildren().cbegin(), 
				getChildren().cend(), 
				element);

			if (iterator != getChildren().cend())
			{
				(*iterator)->size.popListener(elementSizeListener);
				updateSizeByElementSize(
					-(*iterator)->size.getValue()
				);
				removeElement(iterator);
			}
		}

		void removeAllElements()
		{
			for (ElementBase* const element : getChildren())
			{
				element->size.popListener(elementSizeListener);
			}

			removeAllChildren();
		}

		void addElement(ElementBase* const element)
		{
			addElement(element, getChildren().size());
		}

		const Container& getElements() const
		{
			return ElementBase::getChildren();
		}

	private:
		std::function<void(const sf::Vector2f, const sf::Vector2f)> elementSizeListener =
			[this](const sf::Vector2f oldSize,
			       const sf::Vector2f newSize)
		{
			if (direction == CommonControlDirection::Horizontal)
			{
				if (oldSize.x != newSize.x)
					sizePreferred =
					{
						sizePreferred->x + newSize.x - oldSize.x,
						sizePreferred->y
					};
			}
			else
			{
				if (oldSize.y != newSize.y)
					sizePreferred =
					{
						sizePreferred->x,
						sizePreferred->y + newSize.y - oldSize.y
					};
			}
		};

		void updateSizeByElementSize(
			const sf::Vector2f elementSize)
		{
			if (direction == CommonControlDirection::Horizontal)
			{
				if (elementSize.x == 0)
				{
					return;
				}

				sizePreferred =
				{
					sizePreferred->x + elementSize.x,
					sizePreferred->y
				};
			}
			else
			{
				if (elementSize.y == 0)
				{
					return;
				}

				sizePreferred = // TODO: PreferredSize ?
				{
					sizePreferred->x,
					sizePreferred->y + elementSize.y
				};
			}
		}

		void removeElement(const Container::const_iterator element)
		{
			removeChild(element);
		}

		void updateElementPosition()
		{
			const size_t size = ElementBase::getChildren().size();

			if (size > 0)
			{
				updateSingleElementPosition(
					ElementBase::getChildren().front()
				);
			}

			for (size_t i = 1; i < size; ++i)
			{
				updateSingleElementPosition(
					ElementBase::getChildren()[i - 1],
					ElementBase::getChildren()[i]
				);
			}
		}

		void updateSingleElementPosition(
			ElementBase* const lastElement,
			ElementBase* const element)
		{
			if (direction == CommonControlDirection::Horizontal)
			{
				// calcluate area offset
				

				element->position =
				{
					lastElement->position.getValue().x
					+ lastElement->size.getValue().x,
					calculateElementAreaOffset(element)
				};
			}
			else
			{
				element->position =
				{
					calculateElementAreaOffset(element),
					lastElement->position.getValue().y
					+ lastElement->size.getValue().y
				};
			}
		}

		void updateSingleElementPosition(
			ElementBase* const element)
		{
			element->position = direction == CommonControlDirection::Horizontal
				? sf::Vector2f(0.f, calculateElementAreaOffset(element))
				: sf::Vector2f(calculateElementAreaOffset(element), 0.f);
		}

		inline float calculateElementAreaOffset(ElementBase* const element)
		{/*
			if (direction == CommonControlDirection::Horizontal)
			{
				return element->area.getValue() == CommonArea::Bottom
					? size.getValue().y - element->size.getValue().y
					: (element->area.getValue() == CommonArea::Center
						? (size.getValue().y - element->size.getValue().y) / 2.f
						: 0.f);
			}
			else
			{
				return element->area.getValue() == CommonArea::Right
					? size.getValue().x - element->size.getValue().x
					: (element->area.getValue() == CommonArea::Center
						? (size.getValue().x - element->size.getValue().x) / 2.f
						: 0.f);
			}
			*/
			return 0.f;
		}

		float takeByDirection(float x, float y) const
		{
			return direction == CommonControlDirection::Horizontal ? x : y;
		}

		const CommonControlDirection direction;
	};
}
