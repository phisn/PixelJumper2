#pragma once

#include <Client/source/menu/MenuBase.h>

#include <Client/source/device/DeviceInterface.h>
#include <Client/source/device/ScreenDevice.h>

namespace Menu
{
	class LogicScrollBar
		:
		public Menu::Base
	{
	public:
		enum class Direction
		{
			Horizontal,
			Vertical
		};

		// Needs view to convert coords
		LogicScrollBar(
			const Direction direction,
			const sf::View* const view)
			:
			direction(direction),
			view(view)
		{
		}

		virtual ~LogicScrollBar() { }

		Direction getDirection() const
		{
			return direction;
		}

		virtual void onEvent(
			const sf::Event event) override;
	protected:
		bool getIsInside() const
		{
			return isInside;
		}

		float
			length,
			currentPosition;

		virtual void onScrollBarMoved() = 0;

		virtual void onMouseEnter() = 0;
		virtual void onMouseLeave() = 0;

		virtual void onMouseClickBegin() = 0;
		virtual void onMouseClickEnd() = 0;

		virtual bool checkIsInside(
			const int x,
			const int y) const = 0;
	private:
		void handleInsidePressed(
			const sf::Event event);
		void handleInsideMove(
			const sf::Event event);
		void handleMoveScrollBar(
			const sf::Event event);

		float beginPosition;
		bool 
			beginInside = false, 
			isInside = false;

		const Direction direction;

		const sf::View* const view;
	};
}