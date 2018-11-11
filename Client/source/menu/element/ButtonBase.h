#pragma once

#include <Client/source/menu/element/ElementBase.h>

namespace Menu
{
	class ButtonBase
		:
		public ElementBase
	{
	public:
		typedef Properties Properties;

		struct Style
		{
			sf::Color
				default_fillColor,
				default_outlineColor,

				enter_fillColor,
				enter_outlineColor,

				click_fillColor,
				click_outlineColor;

			float
				default_outlineThickness,
				enter_outlineThickness,
				click_outlineThickness;
		};

		ButtonBase(
			const Style style)
			:
			style(style)
		{
		}
	protected:
		void setEnterStyle();
		void setDefaultStyle();
		void setClickStyle();

		virtual void onEnter() = 0;
		virtual void onLeave() = 0;

		virtual void onClick() = 0;

		virtual void onClickBegin() = 0;
		virtual void onClickEnd() = 0;

	private:
		const Style style;

		sf::RectangleShape shape;
	};
}
