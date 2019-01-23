#pragma once

#include <Client/source/device/DeviceInterface.h>
#include <Client/source/device/ScreenDevice.h>

#include <Client/source/editor/grid/GridProperties.h>
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
		GridView(
			sf::View* const view)
			:
			view(view)
		{
			setupTexture();
		}

		void setCenter(
			const sf::Vector2f center)
		{
			view->setCenter(center);

			updateBackground();
		}

		void applyView() const;
		void draw() const;

		void zoom(
			const float zoom);

		sf::Vector2f convertPtoC(
			const sf::Vector2i position) const;
		sf::Vector2i convertCtoP(
			const sf::Vector2f position) const;

		void beginMovment(
			const int x,
			const int y)
		{
			movementOffset.x = x;
			movementOffset.y = y;

			movementBeginView = view->getCenter();
		}

		void setMovement(
			const int x,
			const int y)
		{
			const sf::Vector2f destination = convertPtoC(sf::Vector2i(
				x, y
			));

			const sf::Vector2f source = convertPtoC(sf::Vector2i(
				movementOffset.x,
				movementOffset.y
			));

			setCenter(sf::Vector2f(
				movementBeginView.x + (source.x - destination.x),
				movementBeginView.y + (source.y - destination.y)
			));
		}

		void resetSize()
		{
			const sf::Vector2f size = sf::Vector2f(
				(DEVICE::Interface::GetScreen()->getWindow()->getSize().x * view->getViewport().width) / GridProperties.gridBeginZoom,
				(DEVICE::Interface::GetScreen()->getWindow()->getSize().y * view->getViewport().height) / GridProperties.gridBeginZoom
			);

			view->setSize(size);
			view->setCenter(size / 2.f);

			updateBackground();
		}

		const sf::Sprite* getSprite() const;
		int getGridSize() const;
	private:
		sf::Vector2i movementOffset;
		sf::Vector2f movementBeginView;

		void setupTexture();

		void generateBackground(
			bool isHorizontal);
		void updateBackground();

		struct
		{
			int imageSize = GridProperties.imageSize;
			int gridSize = GridProperties.gridSize;
			int gridLengthDiv = GridProperties.gridLengthDiv;

			sf::Image image;
			sf::Texture	texture;
			sf::Sprite sprite;

			sf::Color color = sf::Color::Color(80, 80, 80);
		} background;

		sf::View* const view;
	};

	inline void GridView::applyView() const
	{
		DEVICE::Interface::GetScreen()->applyView(view);
	}

	inline void GridView::draw() const
	{
		DEVICE::Interface::GetScreen()->onDraw(&background.sprite);
	}

	inline sf::Vector2f GridView::convertPtoC(
		const sf::Vector2i position) const
	{
		return DEVICE::Interface::GetScreen()->getWindow()->mapPixelToCoords(position, *view);
	}

	inline sf::Vector2i GridView::convertCtoP(
		const sf::Vector2f position) const
	{
		return DEVICE::Interface::GetScreen()->getWindow()->mapCoordsToPixel(position, *view);
	}

	inline const sf::Sprite* GridView::getSprite() const
	{
		return &background.sprite;
	}

	inline int GridView::getGridSize() const
	{
		return background.gridSize * 2;
	}
}
