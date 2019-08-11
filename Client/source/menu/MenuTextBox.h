#pragma once

#include <Client/source/menu/MenuButtonMaterial.h>
#include <Client/source/menu/MenuLabel.h>
#include <Client/source/menu/MenuRenderContainer.h>

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
					container.sizePreferred = newSize - sf::Vector2f(
						innerOffset->left + innerOffset->right, // + outerOffset->left + outerOffset->right,
						innerOffset->top + innerOffset->bottom  // + outerOffset->left + outerOffset->right
					);
					label.sizePreferred.setY(container.sizePreferred->y);
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
					updateLabelPosition();
					updateCaretPosition();
				});

			text.addListener(
				[this](const std::wstring oldValue,
					   const std::wstring newValue)
				{
					if (label.position->x + label.size->x < container.size->x && label.position->x != 0)
					{
						const float max_offset = label.size->x - container.size->x;

						if (max_offset < 0)
						{
							label.position.setX(0);
						}
						else
						{
							label.position.setX(container.size->x - label.size->x - 2);
						}
					}


					if (newValue.size() < cursorPosition) // overflow
					{
						cursorPosition = newValue.size();
					}
					else
					{
						updateCaretPosition();
					}
				});

			label.limitWidth = false;

			addChild(&container);
			container.addChild(&label);

			innerOffset = { 5.f, 5.f, 5.f, 5.f };
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

		void onDraw(sf::RenderTarget* const target) const override
		{
			material.draw(target);
			container.onDraw(target);

			if (convertFullPositionRTV(caret.position).x <= container.size->x
				&& isCaretBlink)
			{
				target->draw(caret);
			}
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

		int convertCursorPosition()
		{
			return text->size() >= cursorPosition 
				? *cursorPosition 
				: text->size() - 1;
		}

		void updateLabelPosition()
		{
			if (const int textCursorPosition = convertCursorPosition();
				textCursorPosition != -1)
			{
				const float charPosition =
					(label.readGlText().findCharacterPos(cursorPosition)
				).x;

				if (charPosition > container.size->x) // right overflow
				{
					label.position.setX(label.position->x + container.size->x - charPosition);
				}
				else if (charPosition < 0) // left overflow
				{
					label.position.setX(label.position->x - charPosition);
				}

				if (label.position->x > 0)
				{
					label.position.setX(0);
				}
			}

			else
				if (*label.position != sf::Vector2f(0, 0))
				{
					label.position = { 0, 0 };
				}
		}

		void updateCaretPosition()
		{
			if (const int textCursorPosition = convertCursorPosition();
				textCursorPosition != -1)
			{

				caret.setPosition(container.convertRcrToReal(
					label.readGlText().findCharacterPos(textCursorPosition)
				));
			}
			else
			{

				caret.setPosition(
					convertPosition(sf::Vector2f(0, 0))
				);
			}
		}

		float getGlyphDistanceTo(
			const int glyphIndex, 
			const sf::Vector2f point) const
		{
			const sf::Vector2f glyphVecDistance = container.convertRcrToReal(
				label.readGlText().findCharacterPos(glyphIndex)
			) - point;
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
			Log::Information(std::to_wstring(text->size()));
			for (int i = 1; i <= text->size(); ++i)
			{
				const float nextDistance = getGlyphDistanceTo(i, point);

				if (nextDistance < maxDistance)
				{
					Log::Information(std::to_wstring(i));
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

	protected:
		RenderContainer<> container;
		Label label;
	};
}
