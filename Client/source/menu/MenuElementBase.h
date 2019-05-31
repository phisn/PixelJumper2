#pragma once

#include <Client/source/menu/MenuCommon.h>
#include <Client/source/menu/MenuProperty.h>
#include <Client/source/logger/Logger.h>

#include <cassert>
#include <functional>
#include <vector>

#include <SFML/Graphics/Color.hpp>
#include <SFML/System.hpp>
#include <SFML/Window/Event.hpp>

namespace Menu
{
	template <typename T>
	class AutomaticProperty
		:
		public Property<T>
	{
	public:
		using Property::operator=;
		using Property::operator->;
		using Property::operator*;

		Property<bool> automatic;

		template <typename S>
		AutomaticProperty& operator=(const S value)
		{
			automatic.setValue(false);
			Property<T>::operator=(value);
		}

		// use setValue for automatic define
	};

	class ElementBase
	{
	public:
		Property<ElementBase*> parent{ NULL };

		ElementBase()
		{
			innerOffset.addListener(
				[this](const Offset oldOffset,
					   const Offset newOffset)
				{
					for (ElementBase* const child : staticChildren)
						child->updateAutomaticSpace();
				});
			size.addListener(
				[this](const sf::Vector2f oldSize,
					   const sf::Vector2f newSize)
				{
					for (ElementBase* const child : staticChildren)
						if (*child->space.automatic)
						{
							child->updateAutomaticSpace();
						}
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
					updateSizeWithSpace();
				});
		}

		~ElementBase()
		{
			Container::const_iterator iterator = getStaticChildren().cbegin();
			for (; iterator != getStaticChildren().cend(); ++iterator)
			{
				removeStaticChild(iterator);
				// delete *iterator;
			}
		}

		ElementBase* getParent() const
		{
			return parent;
		}

		AutomaticProperty<sf::Vector2f> space;

		Property<sf::Vector2f> sizePreferred;
		Property<sf::Vector2f> size;
		Property<sf::Vector2f> position{ sf::Vector2f(0, 0) };

		// Property<CommonArea> area{ CommonArea::Center };
		
		struct Offset
		{
			float left = 0.f, top = 0.f, right = 0.f, bottom = 0.f;
		};

		Property<Offset> outerOffset{ };
		Property<Offset> innerOffset{ };

		/*virtual void updateGraphics()
		{
			static int c = 0;
			if (++c % 1000 == 0)
				Log::Information(std::to_wstring(c));

			for (ElementBase* const element : staticChildren)
			{
				element->updateGraphics();
			}

			updateOwnGraphics();
		}*/

		virtual void initialize()
		{
			for (ElementBase* const element : staticChildren)
				element->initialize();
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

		// virtual void updateOwnGraphics() = 0;

		void addStaticChild(ElementBase* const element)
		{
			element->parent.setValue(this);
			staticChildren.push_back(element);
		}

		void insertStaticChild(
			Container::const_iterator position,
			ElementBase* const element)
		{
			element->parent.setValue(this);
			staticChildren.insert(position, element);
		}

		void removeStaticChild(Container::const_iterator element)
		{
			element.operator*()->parent.setValue(NULL);
			staticChildren.erase(element);
		}

		void removeLastStaticChild()
		{
			staticChildren.back()->parent.setValue(NULL);
			staticChildren.pop_back();
		}

		void removeAllStaticChilds()
		{
			for (ElementBase* const element : staticChildren)
			{
				element->parent.setValue(NULL);
			}

			staticChildren.clear();
		}

		const std::vector<ElementBase*>& getStaticChildren() const
		{
			return staticChildren;
		}

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
				: parent->convertPositionRTV(base);
		}

	private:
		std::vector<ElementBase*> staticChildren;
	};
}

