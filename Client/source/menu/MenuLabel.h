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
					
					updateCharacterSize();

					if (!limitWidth)
					{
						updateTextSize();
					}
				});
			font.addListener(
				[this](const sf::Font* const oldFont,
   					   const sf::Font* const newFont)
				{
					glText.setFont(*font);
				});
			color.addListener(
				[this](const sf::Color oldColor,
					   const sf::Color newColor)
				{
					glText.setFillColor(newColor);
				});
			size.addListener(
				[this](const sf::Vector2f oldSize,
					   const sf::Vector2f newSize)
				{
					updateCharacterSize();
				});
			limitWidth.addListener(
				[this](const bool oldValue,
					   const bool newValue)
				{
					updateCharacterSize();

					if (!newValue)
					{
						updateTextSize();
					}
				});
			style.addListener(
				[this](const sf::Uint32 oldStyle,
					   const sf::Uint32 newStyle)
				{
					glText.setStyle(newStyle);

					updateCharacterSize();

					if (!limitWidth)
					{
						updateTextSize();
					}
				});
			
			limitWidth.sync(space.automatic.x);
		}

		Property<sf::Uint32> style;

		Property<sf::Color> color { sf::Color::White };
		Property<const sf::Font*> font { NULL };

		Property<bool> limitWidth{ false };
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

			updateCharacterSize();

 			if (!limitWidth)
			{
				updateTextSize();
			}
		}

		void onDraw(sf::RenderTarget* const target) const override
		{
			target->draw(glText);
		}
		
		const sf::Text& readGlText() const
		{
			return glText;
		}

	protected:
		virtual void updateOwnGraphics() override
		{
			glText.setPosition(convertPosition({ 0.f, 0.f }));
		}

		virtual void moveOwnGraphics(const sf::Vector2f offset) override
		{
			glText.move(offset);
		}

	private:
		void updateCharacterSize()
		{
			glText.setCharacterSize(size->y * 0.7f);

			if (limitWidth)
			{
				const float width = glText.getLocalBounds().width;

				if (width > size->x)
				{
					glText.setCharacterSize((size->x * size->y) / width * 0.7);
				}
			}
		}

		void updateTextSize()
		{
			sizePreferred.setX(glText.getLocalBounds().width);
		}

		sf::Text glText;
	};
}