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
			space->enable(
			{
				direction == CommonControlDirection::Vertical,
				direction == CommonControlDirection::Horizontal
			});
			size.addListener([this](
				const sf::Vector2f oldSize,
				const sf::Vector2f newSize)
				{
					updateElementPosition();
				});
		}

		void addElement(
			ElementBase* const element,
			const size_t position)
		{
			const Container& container = getStaticChildren();
			assert(position <= container.size());

			if (container.size() > 0)
			{
				updateSingleElementPosition(
					container.back(),
					element);

				Container::const_iterator iterator = container.begin() + position;

				if (iterator == container.cend())
				{
					addStaticChild(element);
				}
				else
				{
					insertStaticChild(iterator, element);
				}
			}
			else
			{
				updateSingleElementPosition(element);
				addStaticChild(element);
			}

			element->size.addListener(elementSizeListener);
			updateSizeByElementSize(element->size);
		}

		void removeElement(
			const size_t position)
		{
			removeElement(getStaticChildren().begin() + position);
		}

		void removeElement(
			const ElementBase* const element)
		{
			Container::const_iterator iterator = std::find(
				getStaticChildren().cbegin(), 
				getStaticChildren().cend(), 
				element);

			if (iterator != getStaticChildren().cend())
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
			for (ElementBase* const element : getStaticChildren())
			{
				element->size.popListener(elementSizeListener);
			}

			removeAllStaticChilds();
		}

		void addElement(ElementBase* const element)
		{
			addElement(element, getStaticChildren().size());
		}

		const Container& getElements() const
		{
			return ElementBase::getStaticChildren();
		}

		virtual void updateGraphics() override
		{
			ElementBase::updateGraphics();
			// updateElementPosition();
		}

	protected:
		virtual void updateOwnGraphics() override
		{
		}

	private:
		std::function<void(const sf::Vector2f, const sf::Vector2f)> elementSizeListener =
			[this](const sf::Vector2f oldSize,
			       const sf::Vector2f newSize)
		{
			if (direction == CommonControlDirection::Horizontal)
			{
				if (oldSize.x != newSize.x)
					size =
					{
						size->x + newSize.x - oldSize.x,
						size->y
					};
			}
			else
			{
				if (oldSize.y != newSize.y)
					size =
					{
						size->x,
						size->y + newSize.y - oldSize.y
					};
			}
		};

		void updateSizeByElementSize(
			const sf::Vector2f elementSize)
		{
			if (elementSize.x == 0.f && elementSize.y == 0.f)
			{
				return;
			}

			if (direction == CommonControlDirection::Horizontal)
			{
				size =
				{
					size->x + elementSize.x,
					size->y
				};
			}
			else
			{
				size =
				{
					size->x,
					size->y + elementSize.y
				};
			}
		}

		void removeElement(const Container::const_iterator element)
		{
			removeStaticChild(element);
		}

		void updateElementPosition()
		{
			const size_t size = ElementBase::getStaticChildren().size();

			if (size > 0)
			{
				updateSingleElementPosition(
					ElementBase::getStaticChildren().front()
				);
			}

			for (size_t i = 1; i < size; ++i)
			{
				updateSingleElementPosition(
					ElementBase::getStaticChildren()[i - 1],
					ElementBase::getStaticChildren()[i]
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
		{
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
		}

		float takeByDirection(float x, float y) const
		{
			return direction == CommonControlDirection::Horizontal ? x : y;
		}

		const CommonControlDirection direction;
	};
}
