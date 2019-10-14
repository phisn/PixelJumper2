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
			elementOffset.addIndependentListener(onContainerContentChanged);
			limitLength.addIndependentListener(onContainerContentChanged);
			innerOffset.addIndependentListener(onContainerContentChanged);
			size.addIndependentListener(onContainerContentChanged);
		}

		void addElement(
			ElementBase* const element,
			const size_t position)
		{
			const Container& container = getChildren();

			if (container.size() > 0)
			{
				Container::const_iterator iterator = container.begin() + position;

				if (iterator == container.cend())
				{
					addChild(element);
					updateNewLastElement();
				}
				else
				{
					insertChild(iterator, element);
					updateElements();
				}
			}

			element->sizePreferred.addIndependentListener(onContainerContentChanged);
			element->outerOffset.addIndependentListener(onContainerContentChanged);
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
				// element needs custom container

				removeElement(iterator);
			}
		}

		void removeAllElements()
		{
			for (ElementBase* const element : getChildren())
			{
				// ...
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

		Property<bool> limitLength{ true };
		Property<float> elementOffset{ 0.f };

	private:
		void removeElement(const Container::const_iterator element)
		{
			removeChild(element);
		}

		void updateNewLastElement()
		{
			const Container& container = getChildren();
			ElementBase* const target = container.back();

			const float lastElementPositionOffset = getNextElementPositionOffset(
				container[container.size() - 2]
			);

			// check if element would overflow size and respace is needed
			if (limitLength &&
				(takeByDirection(size) - (container.size() == 1
					? *elementOffset
					: lastElementPositionOffset)
				< elementOffset + takeByDirection(target->size)
				+ (direction == CommonControlDirection::Horizontal
					? outerOffset->left + outerOffset->right
					: outerOffset->top + outerOffset->bottom))
			   )
			{
				updateElements();
			}
			else
			{
				target->position = invertByDirection(
				{
					lastElementPositionOffset,
					getElementInvertedPositionByArea(target)
				});
			}
		}

		void updateElements()
		{
			if (getChildren().size() == 0)
			{
				return;
			}

			if (limitLength)
			{
				updateLimitedElementSpace();
			}
			else
			{
				updateDefaultElementSpace();
			}

			updateElementPosition();
		}

		void updateLimitedElementSpace()
		{
			// determine defualt spaces
			const float maxDefaultSpace = takeByDirection(size)
				- (direction == CommonControlDirection::Horizontal
					? innerOffset->left + innerOffset->right
					: innerOffset->top + innerOffset->bottom)
				- elementOffset * (getChildren().size() + 1);

			const float maxDefaultSingleSpace = maxDefaultSpace / getChildren().size();

			// create vector sorted by sizepreferred to
			// iterate though and find elements with smaller
			// size than default max (adjust default max by
			// every found element) -> resulting in best
			// space for every element
			std::vector<ElementBase*> sizeSortedElements = getChildren();
			std::sort(sizeSortedElements.begin(), sizeSortedElements.end(),
				[this](const ElementBase* const e1, const ElementBase* const e2) -> bool
				{
					return takeByDirection(e1->getFullSizePreferred())
						< takeByDirection(e2->getFullSizePreferred());
				});

			// block vvv (refactor possible)
			float bigElementSpaceRemaining = maxDefaultSpace;
			float currentElementSingleSpace = maxDefaultSingleSpace;

			int smallElementCount = 0;

			for (ElementBase* const element : sizeSortedElements)
			{
				// ignore automatic sizePreferred by space
				if (takeByDirection(element->sizePreferred) != 0)
				{
					const float elementSizePreferred = takeByDirection(element->getFullSizePreferred());

					if (maxDefaultSingleSpace > elementSizePreferred)
					{
						// count all elements smaller than max space
						++smallElementCount;
						bigElementSpaceRemaining -= elementSizePreferred;

						// adjust max element space to new
						currentElementSingleSpace = bigElementSpaceRemaining
							/ (getChildren().size() - smallElementCount);
					}
					else
					{
						break;
					}

				}
			}

			const float bigElementSingleSpace = currentElementSingleSpace;
			// block end ^^^

			// size for inverted space (horizontal -> vertical (y) space)
			const float spaceInverted = takeByDirectionInverted(size)
				- (direction == CommonControlDirection::Vertical
					? innerOffset->left + innerOffset->right
					: innerOffset->top + innerOffset->bottom);

			for (ElementBase* const element : sizeSortedElements)
			{
				float elementSpace;

				// determine space by (is big element or not)
				// is automatic
				if (takeByDirection(element->sizePreferred) == 0)
				{
					elementSpace = bigElementSingleSpace;
				}
				else
				{
					const float elementSizePreferred = takeByDirection(element->getFullSizePreferred());

					// is small element
					if (maxDefaultSingleSpace > elementSizePreferred)
					{
						elementSpace = element->sizePreferred->x;
					}
					else // big
					{
						elementSpace = bigElementSingleSpace;
					}
				}

				// apply space
				element->space = invertByDirection(
					{
						elementSpace,
						spaceInverted
					});
			}
		}

		void updateDefaultElementSpace()
		{
			const float spaceInverted = takeByDirectionInverted(size)
				- (direction == CommonControlDirection::Vertical
					? innerOffset->left + innerOffset->right
					: innerOffset->top + innerOffset->bottom);

			for (ElementBase* const element : getChildren())
			{
				element->space = invertByDirection(
				{
					0.f,
					spaceInverted
				});
			}
		}

		void updateElementPosition()
		{
			const float elementInvertedOffset = (direction == CommonControlDirection::Horizontal
				? innerOffset->top
				: innerOffset->left);

			getChildren().front()->position = invertByDirection(
				{
					(direction == CommonControlDirection::Horizontal
						? innerOffset->left
						: innerOffset->top) + elementOffset,
					elementInvertedOffset
				});

			for (int i = 0; i < getChildren().size(); ++i)
			{
				getChildren()[i]->position = invertByDirection(
				{
					getNextElementPositionOffset(getChildren()[i - 1]),
					elementInvertedOffset
				});
			}
		}

		float getElementInvertedPositionByArea(const ElementBase* element)
		{
			return 0.f;
		}

		float getNextElementPositionOffset(const ElementBase* const element)
		{
			return takeByDirection(element->position)
				 + takeByDirection(element->getFullSize())
				 + elementOffset;
		}

	protected:
		std::function<void()> onContainerContentChanged = [this]()
		{
			updateElements();
		};

		float takeByDirection(float x, float y) const
		{
			return direction == CommonControlDirection::Horizontal ? x : y;
		}

		float takeByDirection(const sf::Vector2f v) const
		{
			return direction == CommonControlDirection::Horizontal ? v.x : v.y;
		}

		float takeByDirectionInverted(const sf::Vector2f v) const
		{
			return direction == CommonControlDirection::Horizontal ? v.y : v.x;
		}

		// horizontal is default
		sf::Vector2f invertByDirection(const sf::Vector2f v)
		{
			return direction == CommonControlDirection::Horizontal
				? v
				: sf::Vector2f(v.y, v.x);
		}

		const CommonControlDirection direction;
	};
}
