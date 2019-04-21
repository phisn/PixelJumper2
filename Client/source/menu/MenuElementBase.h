#pragma once

#include <Client/source/menu/MenuCommon.h>
#include <Client/source/menu/MenuProperty.h>

#include <functional>
#include <vector>

#include <SFML/Graphics/Color.hpp>
#include <SFML/System.hpp>
#include <SFML/Window/Event.hpp>

#include <Client/source/logger/Logger.h>

namespace Menu
{
	class ElementBase
	{
	public:
		ElementBase()
		{
			outerOffset.addListener(
				[this](const Offset oldOffset,
					   const Offset newOffset)
				{
					position =
					{
						position.getValue().x + (newOffset.left - oldOffset.left),
						position.getValue().y + (newOffset.top  - oldOffset.top)
					};

					if (parent == NULL)
					{
						updateGraphics();
					}
					else
					{
						parent->updateGraphics();
					}
				});

			innerOffset.addListener(
				[this](const Offset oldOffset,
					   const Offset newOffset)
				{
					updateGraphics();
				});
			position.addListener(
				[this](const sf::Vector2f oldPosition,
					   const sf::Vector2f newPosition)
				{
					updateGraphics();
				});
			size.addListener(
				[this](const sf::Vector2f oldSize,
					   const sf::Vector2f newSize)
				{
					if (parent == NULL)
					{
						updateGraphics();
					}
					else
					{
						parent->updateGraphics();
					}
				});
			area.addListener(
				[this](const CommonArea oldArea,
					   const CommonArea newArea)
				{
					parent->updateGraphics();
				});
		}

		ElementBase* getParent() const
		{
			return parent;
		}

		Property<sf::Vector2f> size;
		Property<sf::Vector2f> position;

		Property<CommonArea> area = CommonArea::Center;
		
		struct Offset
		{
			float left = 0.f, top = 0.f, right = 0.f, bottom = 0.f;
		};

		Property<Offset> outerOffset{ };
		Property<Offset> innerOffset{ };

		virtual void updateGraphics()
		{
			for (ElementBase* const element : staticChildren)
			{
				element->updateGraphics();
			}

			updateOwnGraphics();
		}

		virtual bool initialize()
		{
			for (ElementBase* const element : staticChildren)
				if (!element->initialize())
				{
					return false;
				}

			return true;
		}

		virtual bool create()
		{
			return true;
		}

		virtual void onEvent(const sf::Event event) 
		{
			for (ElementBase* const element : staticChildren)
			{
				element->onEvent(event);
			}
		}
		virtual void onLogic(const sf::Time time) 
		{
			for (ElementBase* const element : staticChildren)
			{
				element->onLogic(time);
			}
		}
		virtual void onDraw() const
		{
			for (ElementBase* const element : staticChildren)
			{
				element->onDraw();
			}
		}

	protected:
		typedef std::vector<ElementBase*> Container;

		virtual void updateOwnGraphics() = 0;

		virtual void onParentChanged()
		{
			for (ElementBase* const element : staticChildren)
			{
				element->onParentChanged();
			}

			updateOwnGraphics();
		}

		void addStaticChild(ElementBase* const element)
		{
			element->parent = this;
			staticChildren.push_back(element);
			element->onParentChanged();
		}

		void insertStaticChild(
			Container::const_iterator position,
			ElementBase* const element)
		{
			element->parent = this;
			staticChildren.insert(position, element);
			element->onParentChanged();
		}

		const std::vector<ElementBase*>& getStaticChildren() const
		{
			return staticChildren;
		}

		// virtual to real
		sf::Vector2f convertPositionVTR(const sf::Vector2f position) const
		{
			sf::Vector2f base = parent == NULL
				? position
				: parent->convertPositionVTR(position);

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

			base.x -= innerOffset.getValue().left;
			base.y -= innerOffset.getValue().top;

			base.x -= this->position.getValue().x;
			base.y -= this->position.getValue().y;

			return parent == NULL
				? base
				: parent->convertPositionVTR(base);
		}

	private:
		ElementBase* parent = NULL;
		std::vector<ElementBase*> staticChildren;
	};
}

