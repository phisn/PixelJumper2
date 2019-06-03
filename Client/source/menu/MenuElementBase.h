#pragma once

#include <Client/source/menu/MenuCommon.h>
#include <Client/source/menu/MenuSpecialProperties.h>
#include <Client/source/logger/Logger.h>

#include <cassert>
#include <functional>
#include <vector>

#include <SFML/Graphics/Color.hpp>
#include <SFML/System.hpp>
#include <SFML/Window/Event.hpp>

namespace Menu
{
	class ElementBase
	{
	public:
		Property<ElementBase*> parent{ NULL };
		typedef std::vector<ElementBase*> Container;

		ElementBase()
		{
#pragma region PropertyListener
			innerOffset.addListener(
				[this](const CommonElementOffset oldOffset,
					   const CommonElementOffset newOffset)
				{
					moveGraphics(sf::Vector2f(
						newOffset.left - oldOffset.left,
						newOffset.top - newOffset.top
					));

					for (ElementBase* const child : children)
						child->updateAutomaticSpace();
				});
			size.addListener(
				[this](const sf::Vector2f oldSize,
					   const sf::Vector2f newSize)
				{
					for (ElementBase* const child : children)
						if (*child->space.automatic)
						{
							child->updateAutomaticSpace();
						}

					onVirtualGraphicsChanged();
				});
			space.addListener(
				[this](const sf::Vector2f oldSpace,
					   const sf::Vector2f newSpace)
				{
					updateSizeWithSpace();
				});
			position.addListener(
				[this](const sf::Vector2f oldPosition,
					   const sf::Vector2f newPosition)
				{
					moveGraphics(newPosition - oldPosition);
					updateSizeWithSpace();
				});
			sizePreferred.addListener(
				[this](const sf::Vector2f oldSize,
					   const sf::Vector2f newSize)
				{
					updateSizeWithSpace();
				});
#pragma endregion
		}

		~ElementBase()
		{
			removeAllChildren();
		}

		AutomaticProperty<sf::Vector2f> space;

		Property<sf::Vector2f> sizePreferred;
		Property<sf::Vector2f> size;
		Property<sf::Vector2f> position{ sf::Vector2f(0, 0) };

		// Property<CommonArea> area{ CommonArea::Center };

		Property<CommonElementOffset> outerOffset{ };
		Property<CommonElementOffset> innerOffset{ };

#pragma region CommonAccess
	public:
		virtual void initialize()
		{
			for (ElementBase* const element : children)
				element->initialize();
		}

		virtual bool create()
		{
			return true;
		}

		virtual void onEvent(const sf::Event event) 
		{
			for (ElementBase* const element : children)
			{
				element->onEvent(event);
			}
		}
		
		virtual void onLogic(const sf::Time time) 
		{
			for (ElementBase* const element : children)
			{
				element->onLogic(time);
			}
		}

		virtual void onDraw() const
		{
			for (ElementBase* const element : children)
			{
				element->onDraw();
			}
		}
#pragma endregion

#pragma region Container
	public:
		void addChild(ElementBase* const element)
		{
			element->parent.setValue(this);
			children.push_back(element);

			if (*element->space.automatic)
			{
				element->updateAutomaticSpace();
			}

			element->updateGraphics();
		}

		void insertChild(
			Container::const_iterator position,
			ElementBase* const element)
		{
			element->parent.setValue(this);
			children.insert(position, element);

			if (*element->space.automatic)
			{
				element->updateAutomaticSpace();
			}

			element->updateGraphics();
		}

		void removeChild(Container::const_iterator element)
		{
			element.operator*()->parent.setValue(NULL);
			children.erase(element);
		}

		void removeLastChild()
		{
			children.back()->parent.setValue(NULL);
			children.pop_back();
		}

		void removeAllChildren()
		{
			for (ElementBase* const element : children)
			{
				element->parent.setValue(NULL);
			}

			children.clear();
		}

		const std::vector<ElementBase*>& getChildren() const
		{
			return children;
		}

	private:
		Container children;
#pragma endregion

#pragma region GraphicsUpdate
	public:
		// callled manually
		virtual void updateGraphics()
		{
			if (graphicsChanged)
			{
				updateOwnGraphics();
			}

			if (childGraphicsChanged)
			{
				for (ElementBase* const element : children)
					element->updateGraphics();
			}
		}

	protected:
		// called by properties used in
		// Region: Conversion
		void moveGraphics(const sf::Vector2f offset)
		{
			moveOwnGraphics(offset);

			for (ElementBase* const child : children)
				child->moveGraphics(offset);
		}

		// TODO: virtual?
		virtual void onVirtualGraphicsChanged()
		{
			this->graphicsChanged = true;

			for (ElementBase* parent = this->parent.getValue()
				; parent != NULL && !parent->childGraphicsChanged
				; parent = parent->parent.getValue())
			{
				parent->childGraphicsChanged = true;
			}
		}

		virtual void updateOwnGraphics() = 0;
		virtual void moveOwnGraphics(const sf::Vector2f) = 0;
	private:
		bool graphicsChanged = false, childGraphicsChanged = false;

#pragma endregion

#pragma region Space
	protected:
		virtual void updateSizeWithSpace()
		{
			const sf::Vector2f realSpace = *space - *position;

			// use max space as default size
			if (sizePreferred->x == 0 || sizePreferred->y == 0)
			{
				size = realSpace;
			}
			else
			{
				size =
				{
					realSpace.x < sizePreferred->x ? realSpace.x : sizePreferred->x,
					realSpace.y < sizePreferred->y ? realSpace.y : sizePreferred->y,
				};
			}
		}

		virtual void updateAutomaticSpace()
		{
			space.setValue(*parent->size - sf::Vector2f(
				parent->innerOffset->left + parent->innerOffset->right,
				parent->innerOffset->top + parent->innerOffset->bottom
			));
		}
#pragma endregion

#pragma region Conversion
	protected:
		// precent to real
		sf::Vector2f convertPosition(const sf::Vector2f position) const
		{
			const sf::Vector2f virtualPosition = 
			{
				size->x * position.x,
				size->y * position.y
			};

			return convertPositionVTR(virtualPosition);
		}


		// virtual to real
		sf::Vector2f convertPositionVTR(const sf::Vector2f position) const
		{
			sf::Vector2f base = parent == NULL
				? position
				: parent->convertPositionVTR(position);

			// all these values have to be updated
			// with moveOwnGraphics on change
			base.x += innerOffset.getValue().left;
			base.y += innerOffset.getValue().top;

			base.x += this->position.getValue().x;
			base.y += this->position.getValue().y;
			// ---

			return base;
		}

		// real to virtual
		sf::Vector2f convertPositionRTV(const sf::Vector2f position) const 
		{
			sf::Vector2f base = position;

			// all these values have to be updated
			// with moveOwnGraphics on change
			base.x -= innerOffset.getValue().left;
			base.y -= innerOffset.getValue().top;

			base.x -= this->position.getValue().x;
			base.y -= this->position.getValue().y;
			// ---

			return parent == NULL
				? base
				: parent->convertPositionRTV(base);
		}
#pragma endregion

	};
}

