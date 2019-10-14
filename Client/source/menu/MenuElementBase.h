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
		ReadOnlyPropertyContainer<Property<ElementBase*>, ElementBase> parent{ NULL };
		typedef std::vector<ElementBase*> Container;

		ElementBase()
			:
			size([this]() -> sf::Vector2f
			{
				return definitionMakeSize();
			}),
			space([this]() -> sf::Vector2f
			{
				return definitionMakeSpace();
			})
		{
#pragma region PropertyInit
			size.addDependence(&outerOffset);
			size.addDependence(&position);
			size.addDependence(&sizePreferred);
			size.addDependence(&space);

			space.addDependence(&parent);

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

						child->space.updateByExternalDependence();
					}
				});
			size.addListener(
				[this](const sf::Vector2f oldSize,
					   const sf::Vector2f newSize)
				{
					for (ElementBase* const child : children)
						child->space.updateByExternalDependence();

					onVirtualGraphicsChanged();
				});
			position.addListener(
				[this](const sf::Vector2f oldPosition,
					   const sf::Vector2f newPosition)
				{
					moveGraphics(newPosition - oldPosition);
				});
#pragma endregion
		}

		~ElementBase()
		{
			removeAllChildren();
		}

		DependentProperty<sf::Vector2f, AutomaticProperty<sf::Vector2f>> space;
		ReadOnlyPropertyContainer<DependentProperty<sf::Vector2f>, ElementBase> size;

		Property<sf::Vector2f> sizePreferred;
		Property<sf::Vector2f> position{ sf::Vector2f(0, 0) };

		Property<CommonHorizontalArea> horizontalArea{ CommonHorizontalArea::Center };
		Property<CommonVerticalArea> verticalArea{ CommonVerticalArea::Center };

		Property<CommonElementOffset> outerOffset{ };
		Property<CommonElementOffset> innerOffset{ };

		sf::Vector2f getFullSize() const
		{
			return sf::Vector2f(
				size->x + outerOffset->left + outerOffset->right,
				size->y + outerOffset->top + outerOffset->bottom);
		}

		sf::Vector2f getFullSizePreferred() const
		{
			return sf::Vector2f(
				sizePreferred->x + outerOffset->left + outerOffset->right,
				sizePreferred->y + outerOffset->top + outerOffset->bottom);
		}

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

		virtual void onDraw(sf::RenderTarget* const target) const
		{
			for (ElementBase* const element : children)
			{
				element->onDraw(target);
			}
		}
#pragma endregion

	public:
		virtual void onEvent(const sf::Event event)
		{
			switch (event.type)
			{
			case sf::Event::MouseButtonPressed:
				if (ElementBase* const child = findTargetChild(
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

		ReadOnlyPropertyContainer<Property<ElementBase*>, ElementBase> strongSelectedChild{ NULL };
		ReadOnlyPropertyContainer<Property<ElementBase*>, ElementBase> weakSelectedChild{ NULL };

		ReadOnlyPropertyContainer<Property<bool>, ElementBase> weakSelected;
		ReadOnlyPropertyContainer<Property<bool>, ElementBase> strongSelected;

	protected:
		void unselectStrong()
		{
			strongSelected = false;

			if (strongSelectedChild != NULL)
			{
				strongSelectedChild->unselectStrong();
				strongSelectedChild = (ElementBase*) NULL;
			}
		}

		void unselectWeak()
		{
			weakSelected = false;

			if (weakSelectedChild != NULL)
			{
				weakSelectedChild->unselectWeak();
				weakSelectedChild = (ElementBase*) NULL;
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

			for (decltype(children)::const_reverse_iterator child = children.crbegin();
				child != children.crend(); ++child)
			{
				if (isTargetChild(position, *child))
					return *child;
			}

			return expected;
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

#pragma region Container
	public:
		void addChild(ElementBase* const element)
		{
			element->parent.setValue(this);
			children.push_back(element);
			element->updateGraphics();
		}

		void insertChild(
			Container::const_iterator position,
			ElementBase* const element)
		{
			element->parent.setValue(this);
			children.insert(position, element);
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
		sf::Vector2f definitionMakeSize()
		{
			sf::Vector2f result = *space - *position - sf::Vector2f(
				outerOffset->left + outerOffset->right,
				outerOffset->top + outerOffset->bottom); // max possible size

			if (sizePreferred->x != 0 && (sizePreferred->x < result.x || result.x <= 0))
			{
				result.x = sizePreferred->x;
			}

			if (sizePreferred->y != 0 && (sizePreferred->y < result.y || result.y <= 0))
			{
				result.y = sizePreferred->y;
			}

			return result;
		}

		sf::Vector2f definitionMakeSpace()
		{
			if (parent.getValue() == NULL)
			{
				return *space;
			}

			sf::Vector2f result;

			if (space.automatic.x)
			{
				result.x = parent->size->x - (parent->innerOffset->left + parent->innerOffset->right);
			}
			else
			{
				result.x = space->x;
			}

			if (space.automatic.y)
			{
				result.y = parent->size->y - (parent->innerOffset->top + parent->innerOffset->bottom);
			}
			else
			{
				result.x = space->y;
			}

			return result;
		}
#pragma endregion

#pragma region Conversion
	public:
		// precent to real
		virtual sf::Vector2f convertPosition(const sf::Vector2f position) const
		{
			const sf::Vector2f virtualPosition = 
			{
				size->x * position.x,
				size->y * position.y
			};

			return convertPositionVTR(virtualPosition);
		}

		// virtual to real
		virtual sf::Vector2f convertPositionVTR(const sf::Vector2f position) const
		{
			sf::Vector2f base = parent == NULL
				? position
				: parent->convertFullPositionVTR(position);

			base.x += this->position.getValue().x;
			base.y += this->position.getValue().y;

			base.x += outerOffset->left;
			base.y += outerOffset->top;

			return base;
		}

		// virtual to real
		virtual sf::Vector2f convertFullPositionVTR(const sf::Vector2f position) const
		{
			sf::Vector2f base = parent == NULL
				? position
				: parent->convertFullPositionVTR(position);

			base.x += innerOffset->left;
			base.y += innerOffset->top;

			base.x += outerOffset->left;
			base.y += outerOffset->top;

			base.x += this->position->x;
			base.y += this->position->y;

			return base;
		}

		// real to virtual
		virtual sf::Vector2f convertPositionRTV(const sf::Vector2f position) const 
		{
			sf::Vector2f base = position;

			base.x -= this->position->x;
			base.y -= this->position->y;

			base.x -= this->outerOffset->left;
			base.y -= this->outerOffset->top;

			return parent == NULL
				? base
				: parent->convertFullPositionRTV(base);
		}

		// real to virtual
		virtual sf::Vector2f convertFullPositionRTV(const sf::Vector2f position) const
		{
			sf::Vector2f base = position;

			base.x -= innerOffset->left;
			base.y -= innerOffset->top;

			base.x -= this->position->x;
			base.y -= this->position->y;

			base.x -= this->outerOffset->left;
			base.y -= this->outerOffset->top;

			return parent == NULL
				? base
				: parent->convertFullPositionRTV(base);
		}
#pragma endregion

	};
}

