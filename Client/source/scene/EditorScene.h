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

					/*
					moveBeginPosition =
						sf::Vector2i(
							event.mouseButton.x,
							event.mouseButton.y);
					moveViewBegin = view.getCenter();
					*/
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
					/*
					const sf::Vector2f posCoords = DEVICE::Interface::getScreen()->getWindow()->mapPixelToCoords(
						sf::Vector2i(
							moveBeginPosition.x - event.mouseMove.x,
							moveBeginPosition.y - event.mouseMove.y),
						view);

					view.setCenter(sf::Vector2f(
						moveViewBegin.x + posCoords.x / 2.f,
						moveViewBegin.y + posCoords.y / 2.f
					));
					
					gridView.background.sprite.setPosition(
						DEVICE::Interface::getScreen()->getWindow()->mapPixelToCoords(
							sf::Vector2i(0.f, 0.f), view));

					adjustSpriteSize(
						sf::Vector2i(0, 0),
						sf::Vector2i(
							1920,
							1080)
					);
					*/

					break;
				}

				break;
			case sf::Event::MouseWheelMoved:
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
		/*
		void adjustSpriteSize(
			const sf::Vector2i offset,
			const sf::Vector2i size)
		{
			gridView.background.sprite.setTextureRect({ 
				-offset.x / 4,
				-offset.y / 4,
				size.x,
				size.y });

			printf("%d, %d\n", offset.x, offset.y);
		}

		void adjustSelectionSize(
			const sf::Vector2f position)
		{
			selection.setSize(sf::Vector2f(
			  	position.x - selection.getPosition().x,
				position.y - selection.getPosition().y)
			);
		}*/

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
