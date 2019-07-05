#pragma once

#include <Client/source/menu/MenuCommon.h>
#include <Client/source/menu/MenuNotifier.h>
#include <Client/source/menu/MenuSpecialProperties.h>
#include <Client/source/logger/Logger.h>

#include <cassert>
#include <functional>
#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

namespace Menu
{
	class ElementBase
	{
		friend class MenuBaseScene;
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
					for (ElementBase* const child : children)
					{
						child->moveGraphics(sf::Vector2f(
							newOffset.left - oldOffset.left,
							newOffset.top - oldOffset.top
						));

						child->updateAutomaticSpace();
					}
				});
			size.addListener(
				[this](const sf::Vector2f oldSize,
					   const sf::Vector2f newSize)
				{
					for (ElementBase* const child : children)
						if (child->space.isAutomatic())
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

	public:
		virtual void onEvent(const sf::Event event)
		{
			switch (event.type)
			{
			case sf::Event::MouseButtonPressed:
				if (ElementBase * const child = findTargetChild(
						event.mouseButton.x,
						event.mouseButton.y,
						strongSelectedChild);
					strongSelectedChild != child)
				{
					if (strongSelectedChild.getValue())
					{
						// strongSelectedChild->onEvent(event); not needed here?
						strongSelectedChild->unselectStrong();
					}

					strongSelectedChild = child;
					if (strongSelectedChild)
						strongSelectedChild->strongSelected = true;
				}

				// fall though
			case sf::Event::TextEntered:
				if (strongSelectedChild)
				{
					strongSelectedChild->onEvent(event);
				}

				break;
			case sf::Event::MouseMoved:
				if (ElementBase* const child = findTargetChild(
						event.mouseMove.x,
						event.mouseMove.y,
						weakSelectedChild); 
					weakSelectedChild != child)
				{
					if (weakSelectedChild.getValue())
					{
						weakSelectedChild->onEvent(event); // fix lost movement bugs
						weakSelectedChild->unselectWeak();
					}

					weakSelectedChild = child;

					if (weakSelectedChild)
						weakSelectedChild->weakSelected = true;
				}

				// fall though
			case sf::Event::EventType::MouseWheelMoved:
			case sf::Event::EventType::MouseWheelScrolled:
				if (weakSelectedChild.getValue())
				{
					weakSelectedChild->onEvent(event);
				}

				break;
			default:
				for (ElementBase* const child : children)
					child->onEvent(event);

				break;
			}
		}

		ReadOnlyProperty<ElementBase, ElementBase*> strongSelectedChild{ NULL };
		ReadOnlyProperty<ElementBase, ElementBase*> weakSelectedChild{ NULL };

		ReadOnlyProperty<ElementBase, bool> weakSelected;
		ReadOnlyProperty<ElementBase, bool> strongSelected;

	protected:
		void unselectStrong()
		{
			strongSelected = false;

			if (strongSelectedChild != NULL)
			{
				strongSelectedChild->unselectStrong();
				strongSelectedChild = (ElementBase*)NULL;
			}
		}

		void unselectWeak()
		{
			weakSelected = false;

			if (weakSelectedChild != NULL)
			{
				weakSelectedChild->unselectWeak();
				weakSelectedChild = (ElementBase*)NULL;
			}
		}

		void selectStrong() // proxy
		{
			strongSelected = true;
		}

		void selectWeak() // proxy
		{
			weakSelected = true;
		}

	private:
		ElementBase* findTargetChild(
			const float x, 
			const float y,
			ElementBase* const expected)
		{
			const sf::Vector2f position = sf::Vector2f(x, y);

			if (expected && isTargetChild(position, expected))
			{
				return expected;
			}

			for (ElementBase* const child : children)
			{
				if (isTargetChild(position, child))
					return child;
			}

			return NULL;
		}

		bool isTargetChild(
			const sf::Vector2f position,
			const ElementBase* const child) const
		{
			const sf::FloatRect childRect = sf::FloatRect(
				child->convertPositionVTR({ }),
				child->size.getValue());

			return childRect.contains(position);
		}

		/*
		void selectWeakChild(const sf::Event event)
		{
			if (ElementBase* const element = findElementByPosition(
				sf::Vector2f(
					event.mouseMove.x,
					event.mouseMove.y)
			); element != NULL)
			{
				if (weakSelectedChild.getValue() != element)
				{
					if (weakSelectedChild.getValue() != NULL)
					{
						weakSelectedChild->onEvent(event);
					}

					weakSelectedChild = element;
					element->weakSelected = true;

					weakSelectedChild->onEvent(event);
				}
			}
			else
			{
				if (weakSelectedChild.getValue() != NULL)
				{
					weakSelectedChild->onEvent(event);

					weakSelectedChild->weakSelected = false;
					weakSelectedChild = (ElementBase*) NULL;
				}
			}
		}

		void selectStrongChild(const sf::Event event)
		{
			// should not call 
			if (ElementBase * const element = findElementByPosition(
				sf::Vector2f(
					event.mouseButton.x,
					event.mouseButton.y)
			); element != NULL)
			{
				if (strongSelectedChild.getValue() != element)
				{
					strongSelectedChild = element;
					element->strongSelected = true;
				}
			}
			else
			{
				if (strongSelectedChild.getValue() != NULL)
				{
					strongSelectedChild->strongSelected = false;
					strongSelectedChild = (ElementBase*) NULL;
				}
			}
		}
		

		ElementBase* findElementByPosition(const sf::Vector2f position) const
		{
			for (ElementBase* const element : children)
			{
				const sf::FloatRect rect{
					element->convertPositionVTR(sf::Vector2f(0.f, 0.f)),
					*element->size };

				if (rect.contains(position))
				{
					return element;
				}
			}

			return NULL;
		}
		*/

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
		// virtual 
		void updateGraphics()
		{
			if (graphicsChanged)
			{
				updateOwnGraphics();

				graphicsChanged = false;
			}

			if (childGraphicsChanged)
			{
				for (ElementBase* const element : children)
					element->updateGraphics();

				childGraphicsChanged = false;
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

		virtual void updateOwnGraphics()
		{
		}
		virtual void moveOwnGraphics(const sf::Vector2f)
		{
		}

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
			sf::Vector2f result = space;

			if (space.automatic->x)
			{
				result.x = parent->size->x - (parent->innerOffset->left + parent->innerOffset->right);
			}

			if (space.automatic->y)
			{
				result.x = parent->size->x - (parent->innerOffset->top + parent->innerOffset->bottom);
			}

			// no check needed, done outside
			space.setValue(result);
		}
#pragma endregion

#pragma region Conversion
	public:
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
				: parent->convertFullPositionVTR(position);

			base.x += this->position.getValue().x;
			base.y += this->position.getValue().y;

			return base;
		}

		// virtual to real
		sf::Vector2f convertFullPositionVTR(const sf::Vector2f position) const
		{
			sf::Vector2f base = parent == NULL
				? position
				: parent->convertFullPositionVTR(position);

			base.x += innerOffset.getValue().left;
			base.y += innerOffset.getValue().top;

			base.x += this->position.getValue().x;
			base.y += this->position.getValue().y;

			return base;
		}

		// real to virtual
		sf::Vector2f convertPositionRTV(const sf::Vector2f position) const 
		{
			sf::Vector2f base = position;

			base.x -= this->position.getValue().x;
			base.y -= this->position.getValue().y;

			return parent == NULL
				? base
				: parent->convertFullPositionRTV(base);
		}

		// real to virtual
		sf::Vector2f convertFullPositionRTV(const sf::Vector2f position) const
		{
			sf::Vector2f base = position;

			base.x -= innerOffset.getValue().left;
			base.y -= innerOffset.getValue().top;

			base.x -= this->position.getValue().x;
			base.y -= this->position.getValue().y;

			return parent == NULL
				? base
				: parent->convertFullPositionRTV(base);
		}
#pragma endregion

	};
}

