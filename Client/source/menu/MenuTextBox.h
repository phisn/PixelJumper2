#pragma once

#include <Client/source/menu/MenuLabel.h>

namespace Menu
{
	namespace TextBoxMaterial
	{

		class DefaultRectangle
		{
		public:
			DefaultRectangle()
			{
				applyEffect(CommonControlEffect::Default);
			}


			bool contains(const sf::Vector2f point) const
			{
				return rect.getGlobalBounds().contains(point);
			}

			void setPosition(const sf::Vector2f position)
			{
				rect.setPosition(position);
			}

			void setSize(const sf::Vector2f size)
			{
				rect.setSize(size);
			}

			void move(const sf::Vector2f offset)
			{
				rect.move(offset);
			}

			void applyEffect(const CommonControlEffect effect)
			{
				rect.setFillColor(
					styles[(int)effect].innerColor);
				rect.setOutlineColor(
					styles[(int)effect].outerColor);
				rect.setOutlineThickness(
					styles[(int)effect].outlineThickness);
			}

			void draw() const
			{
				Device::Screen::Draw(rect);
			}

		private:
			const CommonShapeStyle styles[3] =
			{
				{ sf::Color::Color(50, 50, 50), sf::Color::Color(70, 70, 70), 1 },
				{ sf::Color::Color(50, 50, 50), sf::Color::Color(100, 100, 100), 3 },
				{ sf::Color::Color(50, 50, 50), sf::Color::Color(100, 100, 100), 3 }
			};

			sf::RectangleShape rect;
		};
	}

	class TextBox
		:
		public ElementBase
	{
		Label label;

	public:
		TextBox()
		{
			addChild(&label);
		}

		void onEvent(const sf::Event event) override
		{
			switch (event.type)
			{
			case sf::Event::KeyPressed:
				switch (event.key.code)
				{
				case sf::Keyboard::Key::Delete:
					if (cursorPosition + 1 >= text->length())
					{
						break;
					}

					cursorPosition = *cursorPosition + 1;

					// fall though
				case sf::Keyboard::Key::BackSpace:
				{
					if (cursorPosition == 0)
					{
						break;
					}

					std::wstring temp = *text;
					temp.erase(temp.cbegin() + cursorPosition);
					text = temp;

					cursorPosition = *cursorPosition - 1;
				}
					break;
				case sf::Keyboard::Key::Left:
					if (cursorPosition > 0)
					{
						cursorPosition = *cursorPosition - 1;
					}

					break;
				case sf::Keyboard::Key::Right:
					if (cursorPosition + 1 < text->length())
					{
						cursorPosition = *cursorPosition + 1;
					}

					break;
				case sf::Keyboard::Key::Up:
				case sf::Keyboard::Key::Down:
					break;
				}

				break;
			case sf::Event::TextEntered:
				if (isprint(event.text.unicode))
				{
					std::wstring value = text.getValue();
					value.insert(value.cbegin() + cursorPosition, (wchar_t) event.text.unicode);
					text = value;
				}

				break;
			}
		}

		Property<sf::Uint32> cursorPosition;
		Property<std::wstring>& text = label.text;

	private:
		virtual void updateOwnGraphics() override
		{

		}

		virtual void moveOwnGraphics(const sf::Vector2f offset) override
		{

		}
	};
}
