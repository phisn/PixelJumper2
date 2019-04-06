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
			position.addListener(
				[this](const sf::Vector2f oldPosition,
					   const sf::Vector2f newPosition)
				{
					updateOwnGraphics();
				});
		}

		Property<const sf::Font*> font = NULL;
		Property<std::wstring> text;

		sf::MemoryInputStream mis;
		bool initialize() override
		{
			if (font.getValue() == NULL)
			{
				Log::Error(L"No font selected");

				return false;
			}

			glText.setFont(*font.getValue());

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
				convertPositionVTR(position.getValue())
			);
		}

		sf::Text glText;
	};
}