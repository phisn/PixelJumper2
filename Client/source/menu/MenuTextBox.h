#pragma once

#include <Client/source/menu/MenuButtonMaterial.h>
#include <Client/source/menu/MenuLabel.h>

class A
{
protected:
	int a;
};

namespace Menu
{
	namespace TextBoxMaterial
	{
		class DefaultStaticRectangle
			:
			public ButtonMaterial::DefaultStaticRectangleBase
		{
		public:
			DefaultStaticRectangle()
			{
				initialize();
			}

			void applyEffect(const Menu::CommonControlEffect effect) override
			{
				DefaultStaticRectangleBase::applyEffect(effect);
				currentlySelected = false;
			}

			void applySelectedEffect(const Menu::CommonSelectedEffect effect)
			{
				applyStyle(additionalStyles[(int) effect]);
				currentlySelected = true;
			}

		private:
			Menu::CommonSelectedEffect currentAdditionalEffect;
			bool currentlySelected = false;

			void updateEffect() override
			{
				if (currentlySelected)
				{
					applySelectedEffect(currentAdditionalEffect);
				}
				else
				{
					DefaultStaticRectangleBase::updateEffect();
				}
			}

			const Menu::CommonShapeStyle& findStyle(const Menu::CommonControlEffect effect) const override
			{
				return styles[(int)effect];
			}

			const CommonShapeStyle styles[3] =
			{
				{ sf::Color::Color(50, 50, 50), sf::Color::Color(70, 70, 70), 1 },
				{ sf::Color::Color(50, 50, 50), sf::Color::Color(100, 100, 100), 1 },
				{ sf::Color::Color(50, 50, 50), sf::Color::Color(100, 100, 100), 1 }
			};

			const CommonShapeStyle additionalStyles[3] =
			{
				{ sf::Color::Color(50, 50, 50), sf::Color::Color(100, 100, 100), 1 },
				{ sf::Color::Color(50, 50, 50), sf::Color::Color(100, 100, 100), 1 },
				{ sf::Color::Color(50, 50, 50), sf::Color::Color(100, 100, 100), 1 }
			};
		};
	}

	class Caret
		:
		public sf::Drawable
	{
	public:
		void setLength(const float length)
		{
			this->length = length;
			updateLinesPosition();
		}

		void setPosition(const sf::Vector2f position)
		{
			this->position = position;
			updateLinesPosition();
		}

		void move(const sf::Vector2f offset)
		{
			line[0].position += offset;
			line[1].position += offset;
		}

		void setColor(const sf::Color color)
		{
			line[0].color = color;
			line[1].color = color;
		}
		
	public:
		void updateLinesPosition()
		{
			line[0].position = position;
			line[1].position = position + sf::Vector2f(0.f, length) * 1.3f;
		}

		sf::Vertex line[2] = { };

		float length = 0.f;
		sf::Vector2f position = { };

		void draw(
			sf::RenderTarget& target,
			sf::RenderStates states) const
		{
			target.draw(line, 2, sf::Lines);
		}
	};

	class TextBox
		:
		public ElementBase
	{
		typedef TextBoxMaterial::DefaultStaticRectangle Material;
	public:
		TextBox()
		{
			size.addListener(
				[this](const sf::Vector2f oldSize,
					   const sf::Vector2f newSize)
				{
					label.sizePreferred = newSize - sf::Vector2f(
						innerOffset->left + innerOffset->right,
						innerOffset->top + innerOffset->bottom
					);
				});
			weakSelected.addListener(
				[this](const bool oldValue,
					   const bool newValue)
				{
					updateMaterialEffect();

					if (newValue)
					{
						Device::Screen::Cursor::SetCursor(sf::Cursor::Type::Text);
					}
					else
					{
						Device::Screen::Cursor::SetCursor(sf::Cursor::Type::Arrow);
					}
				});
			strongSelected.addListener(
				[this](const bool oldValue,
					   const bool newValue)
				{
					updateMaterialEffect();
					
					if (!newValue && isCaretBlink)
					{
						toggleCaretBlink();
					}

					updateCaretPosition();
				});
			cursorPosition.addListener(
				[this](const sf::Uint32 oldValue,
					   const sf::Uint32 newValue)
				{
					updateCaretPosition();
				});

			text.addListener(
				[this](const std::wstring oldValue,
					   const std::wstring newValue)
				{
					if (newValue.size() < cursorPosition) // overflow
					{
						cursorPosition = newValue.size();
					}
					else
					{
						updateCaretPosition();
					}
				});

			label.space.automatic.setX(false);
			
			addChild(&label);
			innerOffset = { 5.f, 5.f, 5.f, 5.f };


			space.automatic.setX(false);
		}

		void onEvent(const sf::Event event) override
		{
			static bool mouseButtonPressed = false;

			switch (event.type)
			{
			case sf::Event::MouseButtonPressed:
				mouseButtonPressed = true;

				selectCursorPositionBy(
					sf::Vector2f(
						event.mouseButton.x,
						event.mouseButton.y)
				);

				break;
			case sf::Event::MouseButtonReleased:
				mouseButtonPressed = false;

				break;
			case sf::Event::MouseMoved:
				if (mouseButtonPressed)
					selectCursorPositionBy(
						sf::Vector2f(
							event.mouseMove.x,
							event.mouseMove.y)
					);

				break;
			case sf::Event::KeyPressed:
				onKeyPressed(event.key);

				break;
			case sf::Event::TextEntered:
				onCharacterEntered(event.text.unicode);

				break;
			}
		}

		void onDraw() const override
		{
			material.draw();
			ElementBase::onDraw();

			if (isCaretBlink)
				Device::Screen::Draw(caret);
		}

		void onLogic(const sf::Time time)
		{
			if (*strongSelected)
			{
				static sf::Time counter = sf::Time::Zero;
				counter += time;

				if (counter >= sf::milliseconds(500))
				{
					toggleCaretBlink();
					counter = sf::Time::Zero;
				}
			}
		}

		Property<sf::Uint32> cursorPosition;
		Property<std::wstring>& text = label.text;

	protected:
		virtual void updateOwnGraphics() override
		{
			material.setPosition(convertPosition({ 0, 0 }));
			material.setSize(*size);

			caret.setLength(label.readGlText().getCharacterSize());
			updateCaretPosition();
		}

		virtual void moveOwnGraphics(const sf::Vector2f offset) override
		{
			material.move(offset);
			caret.move(offset);
		}

	private:
		bool isCaretBlink = false;

		void toggleCaretBlink()
		{
			isCaretBlink = !isCaretBlink;
		}

		void updateCaretPosition()
		{
			if (text->size() >= cursorPosition)
			{
				caret.setPosition(
					label.readGlText().findCharacterPos(cursorPosition)
				);
			}
			else
			{
				if (text->size() > 0)
				{
					caret.setPosition(
						label.readGlText().findCharacterPos(text->size() - 1)
					);
				}
				else
				{
					caret.setPosition(
						convertPosition({ 0, 0 })
					);
				}
			}
		}

		float getGlyphDistanceTo(
			const int glyphIndex, 
			const sf::Vector2f point) const
		{
			const sf::Vector2f glyphVecDistance = label.readGlText().findCharacterPos(glyphIndex) - point;
			return sqrtf(
				glyphVecDistance.x * glyphVecDistance.x +
				glyphVecDistance.y * glyphVecDistance.y);
		}

		void selectCursorPositionBy(const sf::Vector2f point)
		{
			if (text->size() == 0)
			{
				return;
			}

			float maxDistance = getGlyphDistanceTo(0, point);
			int maxDistanceIndex = 0;

			for (int i = 1; i < text->size(); ++i)
			{
				const float nextDistance = getGlyphDistanceTo(i, point);

				if (nextDistance < maxDistance)
				{
					maxDistanceIndex = i;
					maxDistance = nextDistance;
				}
			}

			cursorPosition = maxDistanceIndex;
		}

		void updateMaterialEffect()
		{	
			if (*strongSelected)
			{
				material.applySelectedEffect(*weakSelected
					? CommonSelectedEffect::SelectedHover
					: CommonSelectedEffect::Selected
				);
			}
			else
			{
				material.applyEffect(*weakSelected
					? CommonControlEffect::Hover
					: CommonControlEffect::Default
				);
			}
		}

		void onKeyPressed(const sf::Event::KeyEvent key)
		{
			switch (key.code)
			{
			case sf::Keyboard::Key::Delete:
			{
				if (cursorPosition + 1 > text->length())
				{
					break;
				}

				std::wstring temp = *text;
				temp.erase(temp.cbegin() + cursorPosition);
				text = temp;
			}
				break;
			case sf::Keyboard::Key::BackSpace:
			{
				if (cursorPosition == 0)
				{
					break;
				}

				--cursorPosition;

				std::wstring temp = *text;
				temp.erase(temp.cbegin() + cursorPosition);
				text = temp;
			}
				break;
			case sf::Keyboard::Key::Left:
				if (cursorPosition > 0)
				{
					cursorPosition = *cursorPosition - 1;
				}

				break;
			case sf::Keyboard::Key::Right:
				if (cursorPosition + 1 <= text->length())
				{
					cursorPosition = *cursorPosition + 1;
				}

				break;
			case sf::Keyboard::Key::Up:
			case sf::Keyboard::Key::Down:
				break;
			}

		}

		void onCharacterEntered(const sf::Uint32 character)
		{
			if (iswprint((wint_t) character))
			{
				std::wstring value = text.getValue();
				value.insert(value.cbegin() + cursorPosition, (wchar_t)character);
				text = value;

				++cursorPosition;
			}
		}

		bool isValidAdditonalChar(char c)
		{
			return label.readGlText().findCharacterPos(cursorPosition).x > convertFullPositionVTR(label.position).x + label.size->x;
		}

		Caret caret;
		Material material;
		Label label;
	};
}
