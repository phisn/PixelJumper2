#pragma once

#include <Client/source/device/DeviceInterface.h>
#include <Client/source/device/ScreenDevice.h>

#include <Client/source/scene/MainSceneBase.h>

#include <Client/source/editor/GridView.h>
#include <Client/source/editor/Selector.h>
#include <Client/source/editor/World.h>

#include <SFML/Graphics.hpp>

namespace Scene
{
	class Editor
		:
		public MainBase
	{
	public:
		bool onCreate() override
		{
			return true;
		}

		void onRemove() override
		{
		}

		void initialize() override
		{
			gridView.initialize();

			selector.initialize(
				&gridView,
				&world
			);
		}

		void onShow() override { }
		void onHide() override { }
		
		void onEvent(const sf::Event event) override
		{
			switch (event.type)
			{
			case sf::Event::MouseButtonPressed:
				if (event.mouseButton.button == sf::Mouse::Button::Left)
				{
					pressed = Pressed::Left;
					selector.begin(sf::Vector2f(
						event.mouseButton.x,
						event.mouseButton.y
					));

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
					gridView.resetZoom();
				}

				break;
			case sf::Event::MouseButtonReleased:
				if (pressed == Pressed::Left)
				{
					selector.release(sf::Vector2f(
						event.mouseButton.x,
						event.mouseButton.y));
				}

				pressed = Pressed::None;

				break;
			case sf::Event::MouseMoved:
				if (pressed == Pressed::Left)
				{
					selector.move(sf::Vector2f(
						event.mouseMove.x,
						event.mouseMove.y));

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
			gridView.applyView();

			world.draw();
			gridView.draw();

			DEVICE::Interface::getScreen()->getWindow()->setView(
				DEVICE::Interface::getScreen()->getWindow()->getDefaultView());

			selector.draw();
		}
	private:
		EDITOR::GridView gridView;
		EDITOR::Selector selector;
		EDITOR::World world;

		enum class Pressed
		{
			None,

			Left,
			Middle,
			Right

		} pressed;
	};
}
