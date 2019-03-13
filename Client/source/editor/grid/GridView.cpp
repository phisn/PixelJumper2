#include "GridView.h"

namespace Editor
{
	void GridView::zoom(
		const float zoom)
	{
		view->zoom(zoom);

		updateBackground();
	}

	void GridView::setupTexture()
	{
		background.image.create(
			background.imageSize,
			background.imageSize);

		generateBackground(
			true);  // generate horizontal
		generateBackground(
			false); // generate vertical

		background.texture.loadFromImage(
			background.image);
		background.texture.setRepeated(
			true);

		background.sprite.setTexture(
			background.texture);
	}
	
	void GridView::generateBackground(bool isHorizontal)
	{

		const int gridDistance = (background.imageSize / background.gridSize);

		const int crossLength = (gridDistance / background.gridLengthDiv) * 2;
		const int crossOffset = crossLength / 2;

		for (int a = 0; a < background.gridSize; ++a)
			for (int b = 0; b < background.imageSize; ++b)
			{
				if ((int)(b + crossOffset) % gridDistance > crossLength)
					continue;

				background.image.setPixel(
					isHorizontal ? b : a * gridDistance,
					isHorizontal ? a * gridDistance : b,

					background.color
				);
			}

	}

	void GridView::updateBackground()
	{
		const sf::Vector2f position1 = mapPixelToCoords(
			sf::Vector2i(0, 0)
		);

		background.sprite.setPosition(
			std::floorf(position1.x),
			std::floorf(position1.y) // try offset
		);

		background.sprite.setTextureRect(
			{
				 (int) position1.x,
				 (int) position1.y,
				 (int) view->getSize().x,
				 (int) view->getSize().y
			});
	}
}
