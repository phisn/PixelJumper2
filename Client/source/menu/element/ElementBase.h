#pragma once

#include <Client/source/device/DeviceInterface.h>
#include <Client/source/device/ScreenDevice.h>

#include <SFML/Graphics.hpp>

#ifndef MENU
#define MENU ::Menu
#endif

namespace Menu
{
	/*
	
		How menus work:
			-	ElementBase is the base
				for everything
			-	Each logic provides pure 
				functions
			-	Each implementation has
				properties and styles
			-	Each policy has no styles
				and follows the rules
				declared in the impl.
			-	Policies to not let any
				pure functions open

			ElementBase -> Logic -> [Implementation] -> Policy
			
			-	Policies can be used by 
				components like 
				VerticalScrollContainer 
				or as raw elements
	
	*/

	class ElementBase
	{
	public:
		struct Properties
		{
			sf::View* view;
			sf::Vector2f
				size,
				position;
		};

		virtual ~ElementBase() { }

		// Can't fail & Can poly
		virtual void initialize(
			Properties* const properties);

		virtual void onEvent(
			const sf::Event event) = 0;
		virtual void onLogic(
			const sf::Time time) = 0;
		virtual void onDraw() = 0;

		void setParent(
			ElementBase* const parent);
		void setSpecificOffset(
			const sf::Vector2f offset);

		sf::Vector2f getSize() const { return size; }
		sf::Vector2f getPosition() const { return position; }

		virtual void resetPosition();

		bool isUseOnLogic() const;
		bool isUseOnEvent() const;

	protected:
		sf::Vector2f convertPixel(
			const int x,
			const int y) const;
		sf::Vector2f convertPosition(
			const sf::Vector2f position) const;
		sf::Vector2f convertPosition(
			const float x,
			const float y) const;

		bool useOnEvent = false;
		bool useOnLogic = false;

		ElementBase* getParent() const;
		sf::View* getView() const;
	private:
		sf::Vector2f 
			specificOffset,
			
			nativePosition,
			size,

			position;

		sf::View* view = NULL;
		ElementBase* parent = NULL;
	};

	inline void ElementBase::initialize(
		ElementBase::Properties* const properties)
	{
		view = properties->view;

		size = properties->size;
		nativePosition = properties->position;

		resetPosition();
	}

	inline void ElementBase::setParent(
		ElementBase* const parent)
	{
		this->parent = parent;

		resetPosition();
	}

	inline void ElementBase::setSpecificOffset(
		const sf::Vector2f offset)
	{
		this->specificOffset = offset;

		resetPosition();
	}

	inline void ElementBase::resetPosition()
	{
		position = convertPosition( nativePosition );
	}
	
	inline bool ElementBase::isUseOnLogic() const
	{
		return useOnLogic;
	}
	
	inline bool ElementBase::isUseOnEvent() const
	{
		return useOnEvent;
	}

	inline sf::Vector2f ElementBase::convertPixel(
		const int x,
		const int y) const
	{
		return DEVICE::Interface::getScreen()->getWindow()->mapPixelToCoords(
			sf::Vector2i(x, y), *view
		);
	}
	
	inline sf::Vector2f ElementBase::convertPosition(
		const sf::Vector2f position) const
	{
		if (parent == NULL)
		{
			return position + specificOffset;
		}
		else
		{
			return position + parent->getPosition() + specificOffset;
		}
	}
	
	inline sf::Vector2f ElementBase::convertPosition(
		const float x, 
		const float y) const
	{
		return convertPosition(
			sf::Vector2f(x, y)
		);
	}
	
	inline ElementBase* ElementBase::getParent() const
	{
		return parent;
	}

	inline sf::View* ElementBase::getView() const
	{
		return view;
	}
}
