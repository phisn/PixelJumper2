#pragma once

#include <Client/source/editor/selector/SelectorView.h>
#include <Client/source/menu/MenuRootBase.h>

#include <Client/source/editor/manipulator/cache/CacheManager.h>
#include <Client/source/editor/manipulator/Manipulator.h>

namespace Editor
{
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

			sf::RectangleShape rect;
			rect.setPosition({ 0, 0 });
			rect.setSize({ selectorView.GridRectSize, selectorView.GridRectSize });

			rect.setFillColor(sf::Color::Color(255, 255, 255, 150));

			Device::Screen::Draw(rect);
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
		SelectorMarker selectorMarker;
	};
}
