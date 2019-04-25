#pragma once

#include "MenuElementBase.h"

#include <Client/source/device/ScreenDevice.h>
#include <Client/source/logger/Logger.h>
#include <Client/source/resource/ResourceInterface.h>
#include <Client/source/framework/FrameworkInterface.h>
#include <SFML/Graphics.hpp>

#include <fstream>
#include <string>

namespace Menu
{
	class Label : public ElementBase
	{
	public:
		Label()
		{
			text.addListener(
				[this](const std::wstring oldText,
					   const std::wstring newText)
				{
					glText.setString(newText);
					size =
					{
						glText.getGlobalBounds().width,
						glText.getGlobalBounds().height
					};
				});
			font.addListener(
				[this](const sf::Font* const oldFont,
   					   const sf::Font* const newFont)
				{
					glText.setFont(*font);
					size =
					{
						glText.getLocalBounds().width,
						glText.getLocalBounds().height * 2
					};
				});
			color.addListener(
				[this](const sf::Color oldColor,
					   const sf::Color newColor)
				{
					glText.setFillColor(newColor);
				});
			/* TODO: Cope with that

				size.addListener(
					[this](const sf::Vector2f oldSize,
						const sf::Vector2f newSize)
					{
					});
			*/
		}

		Property<sf::Color> color { sf::Color() };
		Property<const sf::Font*> font { NULL };
		Property<std::wstring> text { L"" };

		bool initialize() override
		{
			if (font.getValue() == NULL)
			{
				Log::Warning(L"No font selected, using default");
				font = Framework::Interface::GetResource<sf::Font>(Resource::Static::DefaultFont);

				return true;
			}

			return true;
		}

		void onDraw() const override
		{
			Device::Screen::Draw(glText);
		}

	private:
		void updateOwnGraphics() override
		{
			glText.setPosition(
				convertPositionVTR({ 0.f, 0.f })
			);
		}

		sf::Text glText;
	};
}