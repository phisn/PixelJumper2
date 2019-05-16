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
	class SpaceProperty
		:
		public CustomProperty
	{
	public:
		void enable(const sf::Vector2<bool> use)
		{
			this->use = use;
			onValueChanged();
		}

		void update(
			const sf::Vector2f size,
			const sf::Vector2f innerOffset)
		{
			space = size - innerOffset;
			onValueChanged();
		}

		sf::Vector2<bool> getUse() const
		{
			return use;
		}

		sf::Vector2f get() const
		{
			return space;
		}
		
		bool validate(
			const sf::Vector2f position,
			const sf::Vector2f size)
		{
			return correctSize(position, size) == size;
		}

		sf::Vector2f correctSize(
			const sf::Vector2f position,
			const sf::Vector2f size)
		{
			sf::Vector2f result = size;

			if (position.x + size.x > space.x)
			{
				result.x = space.x - position.x;
			}

			if (position.y + size.y > space.y)
			{
				result.y = space.y - position.y;
			}

			return result;
		}

	private:
		sf::Vector2<bool> use;
		sf::Vector2f space;
	};

	/*class ScreenDependentProperty
		:
		public PercentProperty
	{
	public:
	};*/

	class ElementBase
	{
	public:
		Property<ElementBase*> parent{ NULL };

		ElementBase()
		{
			outerOffset.addListener(
				[this](const Offset oldOffset,
					   const Offset newOffset)
				{
					position =
					{
						position->x + (newOffset.left - oldOffset.left),
						position->y + (newOffset.top  - oldOffset.top)
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
					space->update(size.getValue(), { newOffset.left, newOffset.top });
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
					if (parent.getValue() == NULL)
					{
						updateGraphics();
					}
					else
					{
						/*assert(
							parent->size->x - position->x >= size->x &&
							parent->size->y - position->y >= size->y);*/

						parent->updateGraphics();
					}

					space->update(newSize, { innerOffset->left, innerOffset->top });
				});
			area.addListener(
				[this](const CommonArea oldArea,
					   const CommonArea newArea)
				{
					parent->updateGraphics();
				});
			parent.addListener(
				[this](ElementBase* const oldElement,
					   ElementBase* const newElement)
				{
					if (oldElement)
					{
						oldElement->space.addListener(onParentSpaceChangedLambda);
					}

					if (newElement)
					{
						newElement->space.addListener(onParentSpaceChangedLambda);
						onParentSpaceChanged(*newElement->space);
					}

					updateGraphics();
				});
		}

		~ElementBase()
		{
			Container::const_iterator iterator = getStaticChildren().cbegin();
			for (; iterator != getStaticChildren().cend(); ++iterator)
			{
				removeStaticChild(iterator);
				delete *iterator;
			}
		}

		ElementBase* getParent() const
		{
			return parent;
		}

		Property<SpaceProperty> space;
		Property<sf::Vector2f> size;
		Property<sf::Vector2f> position{ sf::Vector2f(0, 0) };

		Property<CommonArea> area{ CommonArea::Center };
		
		struct Offset
		{
			float left = 0.f, top = 0.f, right = 0.f, bottom = 0.f;
		};

		Property<Offset> outerOffset{ };
		Property<Offset> innerOffset{ };

		virtual void updateGraphics()
		{
			static int c = 0;
			if (++c % 1000 == 0)
				Log::Information(std::to_wstring(c));
			for (ElementBase* const element : staticChildren)
			{
				element->updateGraphics();
			}

			updateOwnGraphics();
		}

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

		virtual void updateOwnGraphics() = 0;

		// providing virtual function for EASY access
		virtual void onParentSpaceChanged(const SpaceProperty& space)
		{

		}

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
		decltype(space)::Listener onParentSpaceChangedLambda = [this](
			const SpaceProperty& space)
		{
			this->onParentSpaceChanged(space);
		};

		std::vector<ElementBase*> staticChildren;
	};
}

