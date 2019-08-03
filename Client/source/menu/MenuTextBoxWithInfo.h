#pragma once

#include <Client/source/menu/MenuTextBox.h>

namespace Menu
{
	class TextBoxWithInfo
		:
		public TextBox
	{
	public:
		TextBoxWithInfo()
			:
			TextBox()
		{
			label.limitWidth.addListener(
				[this](const bool oldValue,
					   const bool newValue)
				{
					info.limitWidth = newValue;
				});
			label.font.addListener(
				[this](const sf::Font* oldFont,
					   const sf::Font* newFont)
				{
					info.font = newFont;
				});
			label.color.addListener(
				[this](const sf::Color oldColor,
					   const sf::Color newColor)
				{
					const sf::Color expectedColor = newColor - infoColorOffset;

					info.color = (expectedColor.toInteger() > newColor.toInteger())
						? sf::Color::Black
						: expectedColor;
				});

			container.addChild(&info);
		}
	
		Property<std::wstring>& infoText = info.text;

	private:
		sf::Color infoColorOffset = sf::Color(20, 20, 20, 0);
		Label info;
	};
}
