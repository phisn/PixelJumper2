#pragma once

#include <SFML/System.hpp>
#include <SFML/Window/Event.hpp>

#include <functional>
#include <vector>

namespace Menu
{
	template <typename T>
	class Property
	{
		typedef std::function<
			void(const T oldValue, const T newValue)
		> Listener;
	public:
		Property()
		{
		}

		Property(const T value)
			:
			value(value)
		{
		}

		void addListener(const Listener listener)
		{
			listeners.push_back(listener);
		}

		bool popListener(const Listener listener)
		{
			decltype(listeners)::const_iterator iterator = listeners.cbegin();

			do
			{
				if (iterator->target<std::size_t>() == listener.target<std::size_t>())
				{
					listeners.erase(iterator);
					return true;
				}

			} while (++iterator != listeners.cend());

			return false;
		}
		
		Property& operator=(const T value)
		{
			return operator=<T>(value);
		}

		template <typename S>
		Property& operator=(const S value)
		{
			const T temp = this->value;
			this->value = T(value);

			valueChanged(temp);

			return *this;
		}

		operator const T() const
		{
			return value;
		}

		const T getValue() const
		{
			return value;
		}

#ifdef DANGEROUS // TODO: ?
		T& writeValue()
		{
			return value;
		}
#endif

		void valueChanged(const T oldValue) const
		{
			for (const Listener& listener : listeners)
			{
				listener(oldValue, value);
			}
		}

	private:
		std::vector<Listener> listeners;
		T value;
	};

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

					parent->updateGraphics();
				});

			outerOffset.addListener(
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
					updateGraphics();
				});
		}

		ElementBase* getParent() const
		{
			return parent;
		}

		void addStaticChild(ElementBase* const element)
		{
			element->parent = this;
			staticChildren.push_back(element);
			element->onParentChanged();
		}

		Property<sf::Vector2f> size = { };
		Property<sf::Vector2f> position = { };
		
		struct Offset
		{
			float left, top, right, bottom;
		};

		Property<Offset> outerOffset = { };
		Property<Offset> innerOffset = { };

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
		virtual void updateOwnGraphics() = 0;

		virtual void onParentChanged()
		{
			for (ElementBase* const element : staticChildren)
			{
				element->onParentChanged();
			}

			updateOwnGraphics();
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

