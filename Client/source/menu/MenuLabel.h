#pragma once

#include "MenuElementBase.h"

#include <Client/source/logger/Logger.h>

#include <Client/source/device/ScreenDevice.h>
#include <Client/source/framework/FrameworkInterface.h>
#include <Client/source/resource/ResourceInterface.h>

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

					if (glText.getFont() != NULL)
					{
						sizePreferred =
						{
							glText.getGlobalBounds().width,
							glText.getGlobalBounds().height
						};
					}
				});
			font.addListener(
				[this](const sf::Font* const oldFont,
   					   const sf::Font* const newFont)
				{
					glText.setFont(*font);
					sizePreferred =
					{
						glText.getLocalBounds().width,
						glText.getLocalBounds().height
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

			glText.setCharacterSize(18);
		}

		Property<sf::Color> color { sf::Color::White };
		Property<const sf::Font*> font { NULL };
		Property<std::wstring> text { L"" };

		void initialize() override
		{
			if (font.getValue() == NULL)
			{
				Log::Warning(L"No font selected, using default");

				font = Framework::Interface::GetResource<sf::Font>(
					Resource::Static::DefaultFont
				);
			}
		}

		void onDraw() const override
		{
			Device::Screen::Draw(glText);
		}

	protected:
		virtual void updateOwnGraphics() override
		{
			glText.setPosition(convertPositionVTR({ 0.f, 0.f }));
		}

		virtual void moveOwnGraphics(const sf::Vector2f offset) override
		{
			glText.move(offset);
		}

	private:
		sf::Text glText;
	};
}