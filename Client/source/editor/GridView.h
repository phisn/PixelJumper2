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

		void setCenter(
			const sf::Vector2f center)
		{
			view.setCenter(center);

			updateBackground();
		}

		void applyView() const;
		void draw() const;

		void zoom(
			const float zoom);
		void resetZoom(
			const sf::Vector2f size = sf::Vector2f(192, 108));

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

			movementBeginView = view.getCenter();
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

		sf::View* getView();
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

	inline void GridView::applyView() const
	{
		DEVICE::Interface::getScreen()->applyView(&view);
	}

	inline void GridView::draw() const
	{
		DEVICE::Interface::getScreen()->onDraw(&background.sprite);
	}

	inline sf::Vector2f GridView::convertPtoC(
		const sf::Vector2i position) const
	{
		return DEVICE::Interface::getScreen()->getWindow()->mapPixelToCoords(position, view);
	}

	inline sf::Vector2i GridView::convertCtoP(
		const sf::Vector2f position) const
	{
		return DEVICE::Interface::getScreen()->getWindow()->mapCoordsToPixel(position, view);
	}

	inline void GridView::resetZoom(
		const sf::Vector2f size)
	{
		view.setSize(
			size);
		view.setCenter(
			size / 2.f);

		updateBackground();
	}

	inline sf::View * GridView::getView()
	{
		return &view;
	}

	inline const sf::Sprite * GridView::getSprite() const
	{
		return &background.sprite;
	}

	inline int GridView::getGridSize() const
	{
		return background.gridSize * 2;
	}
}
