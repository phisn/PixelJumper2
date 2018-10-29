#pragma once

#include <Client/source/device/DeviceInterface.h>
#include <Client/source/device/ScreenDevice.h>

#include <Client/source/scene/MainSceneBase.h>

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
			selection.setFillColor(sf::Color(0.f, 0.f, 0.f, 0.f));
			selection.setOutlineColor(sf::Color::Blue);
			selection.setOutlineThickness(2.f);

			view.setSize(sf::Vector2f(
				1920,
				1080));

			image.create(1920.f, 1080.f);
			for (float x = 1; x < 19.1f; ++x)
			{
				for (float y = 1; y < 1080.f; ++y)
				{
					if ((int) (y + 20) % 100 > 40)
					{
						continue;
					}

					image.setPixel(
						x * 100,
						y,
						sf::Color::Color(80, 80, 80)
					);
				}
			}

			for (float y = 1; y < 10.7f; ++y)
			{
				for (float x = 1; x < 1920.f; ++x)
				{
					if ((int) (x + 20) % 100 > 40)
					{
						continue;
					}

					image.setPixel(
						x,
						y * 100,
						sf::Color::Color(80, 80, 80)
					);
				}
			}

			image.saveToFile("testFile.png");
			texture.loadFromImage(image);
			sprite.setTexture(texture, true);

			return true;
		}

		void onRemove() override
		{

		}

		void initialize() override
		{
		}

		void onShow() override { }
		void onHide() override { }
		
		void onEvent(const sf::Event event) override
		{
			switch (event.type)
			{
			case sf::Event::MouseButtonPressed:
				pressed = true;
				selection.setPosition(DEVICE::Interface::getScreen()->getWindow()->mapPixelToCoords(
					sf::Vector2i(
						event.mouseButton.x,
						event.mouseButton.y),
					view));
				selection.setSize(sf::Vector2f());

				break;
			case sf::Event::MouseButtonReleased:
				pressed = false;
				adjustSelection(DEVICE::Interface::getScreen()->getWindow()->mapPixelToCoords(
					sf::Vector2i(
						event.mouseButton.x,
						event.mouseButton.y),
					view));

				break;
			case sf::Event::MouseMoved:
				if (pressed)
				{
					adjustSelection(DEVICE::Interface::getScreen()->getWindow()->mapPixelToCoords(
						sf::Vector2i(
							event.mouseMove.x,
							event.mouseMove.y),
						view));
				}

				break;
			}
		}

		void onLogic(
			const sf::Time time) override
		{
			
		}

		void onDraw() override
		{
			DEVICE::Interface::getScreen()->onDraw(&sprite);

			DEVICE::Interface::getScreen()->onDraw(&selection);
		}
	private:
		void adjustSelection(
			const sf::Vector2f position)
		{
			selection.setSize(sf::Vector2f(
			  	position.x - selection.getPosition().x,
				position.y - selection.getPosition().y)
			);
		}

		sf::RectangleShape selection;
		sf::Vector2f
			selectionPos1,
			selectionPos2;
		sf::Image image;
		sf::Texture texture;
		sf::Sprite sprite;

		sf::View view;

		bool pressed;
	};
}
