#pragma once

#include <Client/source/menu/MenuBase.h>

namespace Menu
{
	class LogicButton
		:
		public Base
	{
	public:
		LogicButton()
		{
			useOnEvent = true;
		}

		virtual ~LogicButton() { }

		virtual void onEvent(
			const sf::Event event) override;
	protected:
		virtual void onMouseEnter() = 0;
		virtual void onMouseLeave() = 0;

		virtual void onMouseClick() = 0;
		
		virtual void onMouseClickBegin() = 0;
		virtual void onMouseClickEnd() = 0;

		virtual bool checkIsInside(
			const int x,
			const int y) const = 0;

		bool getIsInside() const
		{
			return isInside;
		}
	private:
		bool isInside = false, beginInside = false;
	};
}
