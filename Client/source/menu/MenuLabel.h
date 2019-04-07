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
						glText.getGlobalBounds().width,
						glText.getGlobalBounds().height
					};
				});
			size.addListener(
				[this](const sf::Vector2f oldSize,
					const sf::Vector2f newSize)
				{
					Log::Warning(L"Someone tried to change a Label size to '" 
						+ std::to_wstring(newSize.x) + L"|"
						+ std::to_wstring(newSize.y) + L"' (text: '"
						+ glText.getString() + L"')");
				});
		}

		Property<const sf::Font*> font { NULL };
		Property<std::wstring> text { L"" };

		bool initialize() override
		{
			if (font.getValue() == NULL)
			{
				Log::Warning(L"No font selected, using default");
				font = Framework::Interface::GetResource<sf::Font>(Resource::Static::DefaultFont);

				return false;
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

			/*
				static int counter = 0;
				Log::Information(std::to_wstring(++counter));
			*/
		}

		sf::Text glText;
	};
}