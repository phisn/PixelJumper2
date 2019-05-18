#pragma once

#include <Client/source/device/ScreenDevice.h>
#include <Client/source/menu/MenuCommon.h>

#include <SFML/System/Vector2.hpp>

namespace Menu::ButtonMaterial
{
	class DefaultCircle
	{
	public:
		DefaultCircle()
			:
			circle(0.f, 50)
		{
			applyEffect(CommonControlEffect::Default);
		}

		bool contains(const sf::Vector2f point) const
		{
			const sf::Vector2f size = { circle.getGlobalBounds().width, circle.getGlobalBounds().height };

			const float xDiff = point.x - (circle.getPosition().x + size.x / 2.f);
			const float yDiff = point.y - (circle.getPosition().y + size.y / 2.f);

			const float xDivR = (size.x * size.x) / 4.f;
			const float yDivR = (size.y * size.y) / 4.f;

			return (xDiff * xDiff) / xDivR + (yDiff * yDiff) / yDivR < 1;
		}

		void setPosition(const sf::Vector2f position)
		{
			circle.setPosition(position);
		}

		void setSize(const sf::Vector2f size)
		{
			circle.setRadius(
				std::min(size.x, size.y) / 2.f
			);
			circle.setScale(
				size.x < size.y ? 1 : size.x / size.y,
				size.y < size.x ? 1 : size.y / size.x
			);
		}

		void applyEffect(const CommonControlEffect effect)
		{
			circle.setFillColor(
				styles[(int)effect].innerColor);
			circle.setOutlineColor(
				styles[(int)effect].outerColor);
			circle.setOutlineThickness(
				styles[(int)effect].outlineThickness);
		}

		void draw() const
		{
			Device::Screen::Draw(circle);
		}

	private:
		const CommonShapeStyle styles[3] =
		{
			{ sf::Color::Color(50, 50, 50), sf::Color::Color(), 0.f },
			{ sf::Color::Color(50, 50, 50), sf::Color::Color(100, 100, 100), 3.f },
			{ sf::Color::Color(80, 80, 80), sf::Color::Color(), 0.f }
		};

		sf::CircleShape circle;
	};

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
			{ sf::Color::Color(50, 50, 50), sf::Color::Color(), 0 },
			{ sf::Color::Color(50, 50, 50), sf::Color::Color(100, 100, 100), 3 },
			{ sf::Color::Color(80, 80, 80), sf::Color::Color(), 0 }
		};

		sf::RectangleShape rect;
	};

	class DefaultRectangleStaticSize
	{
	public:
		DefaultRectangleStaticSize()
		{
			applyEffect(currentEffect = CommonControlEffect::Default);
		}

		bool contains(const sf::Vector2f point) const
		{
			return rect.getGlobalBounds().contains(point);
		}

		void setPosition(const sf::Vector2f position)
		{
			this->position = position;
			applyEffect(currentEffect);
		}

		void setSize(const sf::Vector2f size)
		{
			this->size = size;
			applyEffect(currentEffect);
		}

		void applyEffect(const CommonControlEffect effect)
		{
			const CommonShapeStyle& style = styles[(int) (currentEffect = effect)];

			rect.setPosition(
				position.x + style.outlineThickness,
				position.y + style.outlineThickness);
			rect.setSize(sf::Vector2f(
				size.x - style.outlineThickness * 2,
				size.y - style.outlineThickness * 2));

			rect.setFillColor(
				style.innerColor);
			rect.setOutlineColor(
				style.outerColor);
			rect.setOutlineThickness(
				style.outlineThickness);
		}

		void draw() const
		{
			Device::Screen::Draw(rect);
		}

	private:
		sf::Vector2f position, size;
		CommonControlEffect currentEffect;

		const CommonShapeStyle styles[3] =
		{
			{ sf::Color::Color(50, 50, 50), sf::Color::Color(40, 40, 40), 0.5f },
			{ sf::Color::Color(50, 50, 50), sf::Color::Color(100, 100, 100), 3 },
			{ sf::Color::Color(80, 80, 80), sf::Color::Color(), 0 }
		};

		sf::RectangleShape rect;
	};
}