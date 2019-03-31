#pragma once

#include "MenuElementBase.h"

#include <Client/source/device/ScreenDevice.h>
#include <Client/source/logger/Logger.h>
#include <Client/source/resource/ResourceInterface.h>
#include <Client/source/framework/FrameworkInterface.h>

#include <SFML/Graphics.hpp>

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

		Property<std::wstring> text;

		bool initialize() override
		{

			if (!font.loadFromFile("font.ttf")) // loadFromStream(Framework::Resource::Get(Resource::Static::Id::DefaultFont)))
			{
				Log::Error(L"Failed to load font");

				return false;
			}

			glText.setFont(font);
			glText.setFillColor(sf::Color::White);
			glText.setString(L"Hello World");

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

		sf::Font font;
		sf::Text glText;
	};
}