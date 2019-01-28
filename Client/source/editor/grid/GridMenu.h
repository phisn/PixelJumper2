#pragma once

#include <Client/source/editor/grid/GridView.h>
#include <Client/source/editor/grid/Selector.h>
#include <Client/source/editor/World.h>

#include <Client/source/menu/RootBase.h>

namespace Editor
{
	enum class Pressed
	{
		None,

		Left,
		Middle,
		Right

	};

	class GridMenu
		:
		public MENU::RootBase
	{
	public:
		GridMenu()
			:
			gridView(getView()),
			selector(&gridView)
		{
		}

		~GridMenu() { }

		bool initialize() override { return true; }

		void onEvent(
			const sf::Event event) override
		{
			switch (event.type)
			{
			case sf::Event::MouseButtonPressed:
				if (event.mouseButton.button == sf::Mouse::Button::Left)
				{
					pressed = Pressed::Left;
					selector.begin(
						event.mouseButton.x,
						event.mouseButton.y
					);

					break;
				}

				if (event.mouseButton.button == sf::Mouse::Button::Right)
				{
					pressed = Pressed::Right;

					gridView.beginMovment(
						event.mouseButton.x,
						event.mouseButton.y
					);
				}

				if (event.mouseButton.button == sf::Mouse::Button::Middle)
				{
					gridView.resetSize();
				}

				break;
			case sf::Event::MouseButtonReleased:
				if (pressed == Pressed::Left)
				{
					selector.release();
				}

				pressed = Pressed::None;

				break;
			case sf::Event::MouseMoved:
				if (pressed == Pressed::Left)
				{
					selector.move(
						event.mouseMove.x,
						event.mouseMove.y
					);

					break;
				}

				if (pressed == Pressed::Right)
				{
					gridView.setMovement(
						event.mouseMove.x,
						event.mouseMove.y
					);

					break;
				}

				break;
			case sf::Event::MouseWheelMoved:
				pressed = Pressed::None;
				gridView.zoom(1.f - 0.1f / (float) event.mouseWheel.delta);

				break;
			}
		}

		void onLogic(
			const sf::Time time) override { }

		void onDraw() override
		{
			MENU::RootBase::onDraw();

			gridView.draw();

			for (TileBase* const tile : Manipulator::GetWorld()->getTiles())
			{
				DEVICE::Interface::GetScreen()->onDraw(
					tile->getShape()
				);
			}

			DEVICE::Interface::GetScreen()->getWindow()->setView(
				DEVICE::Interface::GetScreen()->getWindow()->getDefaultView()
			);

			selector.draw();
		}

		void resetLayout() override
		{
			gridView.resetSize();
		}

	private:
		GridView gridView;
		Selector selector;

		Pressed pressed;
	};
}
