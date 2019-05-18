#pragma once

#include <Client/source/device/ScreenDevice.h>

#include <Client/source/menu/MenuRootBase.h>
#include <Client/source/menu/shape/CrossShape.h>

namespace Editor
{
	class SelectorView
	{
	public:
		SelectorView(sf::View& view)
			:
			view(view)
		{
			crossBase.position = { 0.f, 0.f };
			crossBase.size = { 10, 10 };
			crossBase.consumption = 0.05;
			crossBase.color = sf::Color::Color(100, 100, 100);
		}

		void zoom(const float factor)
		{
			if (factor > 1.f)
			{
				if (view.getSize().x < 1000.f && view.getSize().y < 1000.f)
					view.zoom(factor);
			}
			else if(factor < 1.f)
			{
				if (view.getSize().x > 100.f && view.getSize().x > 100.f)
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

		void draw() const
		{
			drawGrid();
		}
		
	private:
		mutable Menu::CrossShape crossBase;
		sf::Vector2f spaceBetweenCross = { 15.f, 15.f };

		void drawGrid() const
		{
			// could be exported vvv
			const sf::Vector2f crossOffset = crossBase.size.getValue() + spaceBetweenCross;
			const sf::Vector2i crossCount =
			{
				(int) (view.getSize().x / crossOffset.x),
				(int) (view.getSize().y / crossOffset.y)
			};

			const sf::Vector2f viewPosition = Device::Screen::GetWindow()->
				mapPixelToCoords(sf::Vector2i(0, 0), view);
			
			sf::Vector2f crossBaseOffset = viewPosition - sf::Vector2f(
				std::fmodf(viewPosition.x, crossOffset.x) - (viewPosition.x > 0 ? crossOffset.x : 0),
				std::fmodf(viewPosition.y, crossOffset.y) - (viewPosition.y > 0 ? crossOffset.y : 0)
			);
			// ^^^ (Performance)

			// make a try with a version, with vertexbuffer and many crosses (memory is rary a problem)
			for (int x = -1; x < crossCount.x + 1; ++x)
				for (int y = -1; y < crossCount.y + 1; ++y)
				{
					drawSingleCross(sf::Vector2f(
						crossOffset.x * x + crossBaseOffset.x,
						crossOffset.y * y + crossBaseOffset.y
					));
				}

			view.getSize();
			view.getCenter();
		}

		void drawSingleCross(const sf::Vector2f position) const
		{
			crossBase.position = position;
			Device::Screen::Draw(crossBase);
		}

		sf::Vector2i beginMouseOffset;
		sf::Vector2f beginViewCenterOffset;

		sf::View& view;
	};

	class SelectorRoot
		:
		public Menu::MenuRootBase
	{
	public:
		SelectorRoot()
			:
			MenuRootBase(),
			selectorView(view)
		{
		}

		bool build() override
		{
			view.setSize({ 200, 200 });
			view.setCenter({ 0, 0 });

			return true;
		}

		void onDraw() const override
		{
			MenuRootBase::onDraw();
			selectorView.draw();
		}

		void onEvent(const sf::Event event) override
		{
			MenuRootBase::onEvent(event);

			switch (event.type)
			{
			case sf::Event::MouseWheelMoved:
				selectorView.zoom(1.f - 0.1f / (float) event.mouseWheel.delta);

				break;
			case sf::Event::MouseButtonPressed:
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					isMouseLeftPressed = true;
					selectorView.beginMouseMovement(
						sf::Vector2i(event.mouseButton.x, event.mouseButton.y)
					);
				}

				break;
			case sf::Event::MouseMoved:
				if (isMouseLeftPressed)
				{
					selectorView.nextMouseMovement(
						sf::Vector2i(event.mouseMove.x, event.mouseMove.y)
					);
				}

				break;
			case sf::Event::MouseButtonReleased:
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					isMouseLeftPressed = false;
				}

				break;
			}
		}

	private:
		bool isMouseLeftPressed = false;
		SelectorView selectorView;
	};
}
