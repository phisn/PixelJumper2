#pragma once

#include <Client/source/device/DeviceInterface.h>
#include <Client/source/device/ScreenDevice.h>

#include <Client/source/editor/tile/TileBase.h>

#include <SFML/Graphics.hpp>

#include <vector>

#ifndef EDITOR
#define EDITOR ::Editor
#endif

namespace Editor
{
	class GridView
	{
	public:
		void initialize()
		{
			setupTexture();

			resetZoom();
		}

		void applyView() const
		{
			DEVICE::Interface::getScreen()->applyView(&view);
		}

		void draw() const
		{
			DEVICE::Interface::getScreen()->onDraw(&background.sprite);
		}

		sf::Vector2f convertPtoC(
			const sf::Vector2i position) const
		{
			return DEVICE::Interface::getScreen()->getWindow()->mapPixelToCoords(position);
		}

		sf::Vector2i convertCtoP(
			const sf::Vector2f position) const
		{
			return DEVICE::Interface::getScreen()->getWindow()->mapCoordsToPixel(position);
		}

		void zoom(
			const float zoom)
		{
			view.zoom(zoom);
		}

		void resetZoom(
			sf::Vector2f size = sf::Vector2f(192, 108)
			)
		{
			view.setSize(size);
			view.setCenter(size / 2.f);
		}

		sf::View* getView()
		{
			return &view;
		}

		const sf::Sprite* getSprite() const
		{
			return &background.sprite;
		}

		int getGridSize() const
		{
			return background.gridSize * 2;
		}
	private:
		void setupTexture()
		{
			background.image.create(
				background.imageSize, 
				background.imageSize);

			generateBackground(
				true);
			generateBackground(
				false);

			background.texture.loadFromImage(
				background.image);
			background.texture.setRepeated(
				true);

			background.sprite.setTexture(
				background.texture);
		}

		void generateBackground(
			bool isHorizontal)
		{
			const int gridDistance = (background.imageSize / background.gridSize);

			const int crossLength = (gridDistance / background.gridLengthDiv) * 2;
			const int crossOffset = crossLength / 2;

			for (int a = 0; a < background.gridSize; ++a)
				for (int b = 0; b < background.imageSize; ++b)
				{
					if ((int) (b + crossOffset) % gridDistance > crossLength)
						continue;

					background.image.setPixel(
						isHorizontal ? b : a * gridDistance,
						isHorizontal ? a * gridDistance : b,

						background.color
					);
				}
		}

		struct
		{
			int imageSize = 200;
			int gridSize = 10;
			int gridLengthDiv = 5;

			sf::Image image;
			sf::Texture	texture;
			sf::Sprite sprite;

			sf::Color color = sf::Color::Color(80, 80, 80);
		} background;

		sf::View view;
	};
}
