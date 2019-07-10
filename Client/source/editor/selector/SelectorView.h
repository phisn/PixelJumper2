#pragma once

#include <Client/source/device/ScreenDevice.h>
#include <Client/source/menu/shape/CrossShape.h>

#include <Client/source/editor/manipulator/EditorCache.h>
#include <Client/source/editor/manipulator/Manipulator.h>

#include <SFML/Graphics.hpp>

#include <Client/source/logger/Logger.h>

namespace Editor
{
	class SelectorView
	{
		static constexpr float GridCrossSize = 10.f;
		static constexpr float GridCrossSpace = 15.f;
	public:
		static constexpr float GridRectSize = GridCrossSize + GridCrossSpace;

		SelectorView(sf::View& view)
			:
			view(view)
		{
			crossBase.position = { 0.f, 0.f };
			crossBase.size = { GridCrossSize, GridCrossSize };
			crossBase.consumption = 0.1f;
			crossBase.color = sf::Color::Color(100, 100, 100, 255);
		}

		void zoom(const float factor)
		{
			if (factor > 1.f)
			{
				if (view.getSize().x < 1000.f && view.getSize().y < 1000.f)
					view.zoom(factor);
			}
			else if (factor < 1.f)
			{
				if (view.getSize().x > 100.f && view.getSize().y > 100.f)
					view.zoom(factor);
			}
		}

		void beginMouseMovement(const sf::Vector2i position)
		{
			beginMouseOffset = position;
			beginViewCenterOffset = view.getCenter();
		}

		void nextMouseMovement(const sf::Vector2i nextPosition)
		{
			const sf::Vector2f nextCoordsPosition =
				Device::Screen::GetWindow()->mapPixelToCoords(nextPosition, view);
			const sf::Vector2f beginCoordsPosition =
				Device::Screen::GetWindow()->mapPixelToCoords(beginMouseOffset, view);

			view.setCenter(
				beginViewCenterOffset + (beginCoordsPosition - nextCoordsPosition)
			);
		}

		void draw(sf::RenderTarget* const target) const
		{
			drawGrid(target);
		}

	private:
		mutable Menu::CrossShape crossBase;

		void drawGrid(sf::RenderTarget* const target) const
		{
			// could be exported vvv
			const sf::Vector2f crossOffset = crossBase.size.getValue()
				+ sf::Vector2f(GridCrossSpace, GridCrossSpace);
			const sf::Vector2i crossCount =
			{
				(int)(view.getSize().x / crossOffset.x),
				(int)(view.getSize().y / crossOffset.y)
			};

			const sf::Vector2f viewPosition = Device::Screen::GetWindow()->
				mapPixelToCoords(sf::Vector2i(0, 0), view);

			sf::Vector2f crossBaseOffset = viewPosition - sf::Vector2f(
				std::fmodf(viewPosition.x, crossOffset.x) - (viewPosition.x > 0 ? crossOffset.x : 0),
				std::fmodf(viewPosition.y, crossOffset.y) - (viewPosition.y > 0 ? crossOffset.y : 0)
			) - crossBase.size.getValue() / 2.f;
			// ^^^ (Performance)

			// make a try with a version, with vertexbuffer and many crosses (memory is rary a problem)
			for (int x = -1; x < crossCount.x + 1; ++x)
				for (int y = -1; y < crossCount.y + 1; ++y)
				{
					drawSingleCross(sf::Vector2f(
						crossOffset.x * x + crossBaseOffset.x,
						crossOffset.y * y + crossBaseOffset.y
					), target);
				}

			view.getSize();
			view.getCenter();
		}

		void drawSingleCross(
			const sf::Vector2f position,
			sf::RenderTarget* const target) const
		{
			crossBase.position = position;
			target->draw(crossBase);
		}

		sf::Vector2i beginMouseOffset;
		sf::Vector2f beginViewCenterOffset;

		sf::View& view;
	};
}
