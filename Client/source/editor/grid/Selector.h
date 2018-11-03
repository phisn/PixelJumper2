#pragma once

#include <SFML/Graphics.hpp>

#include <Client/source/device/DeviceInterface.h>
#include <Client/source/device/ScreenDevice.h>

#include <Client/source/editor/grid/GridView.h>
#include <Client/source/editor/World.h>

namespace Editor
{
	struct Selection
	{
		Selection()
		{
			altSelectionRect.setFillColor(
				sf::Color::Color(100, 100, 100, 80)
			);
		}

		enum class Type
		{
			Random,
			EqualType,
		} type;

		std::vector<TileBase*> tiles;
		sf::RectangleShape altSelectionRect;
	};

	class Selector
	{
	public:
		void initialize(
			GridView* const view,
			World* const world)
		{
			selection = new Selection();

			this->view = view;
			this->world = world;
			
			shape.setFillColor(
				sf::Color::Transparent);
			shape.setOutlineColor(
				color);
			shape.setOutlineThickness(
				2.f);
			selectedTileShape.setFillColor(
				selectedTileColor);
		}

		void begin(
			const sf::Vector2f position)
		{
			shape.setPosition(
				offset = position);
		}

		void move(
			const sf::Vector2f position)
		{
			if (!active)
			{
				active = true;
			}

			shape.setSize(sf::Vector2f(
				position.x - offset.x,
				position.y - offset.y)
			);

			select(
				sf::Vector2i(
					shape.getPosition()),
				sf::Vector2i(
					shape.getSize())
			);
		}

		// return == click
		bool release(
			const sf::Vector2f position)
		{
			move(position);

			if (active)
			{
				active = false;

				return false;
			}

			return true;
		}

		bool isActive() const
		{
			return active;
		}

		void draw()
		{
			if (active)
			{
				DEVICE::Interface::getScreen()->onDraw(&shape);
			}

			view->applyView();

			if (selection->tiles.size() == 0)
			{
				DEVICE::Interface::getScreen()->onDraw(
					&selection->altSelectionRect);
			}
			else
			{
				for (TileBase* tile : selection->tiles)
				{
					selectedTileShape.setPosition(
						tile->getShape()->getPosition());
					selectedTileShape.setSize(
						tile->getShape()->getSize());

					DEVICE::Interface::getScreen()->onDraw(
						tile->getShape());
				}
			}
		}

	private:
		bool active;

		GridView* view;
		World* world;
		Selection* selection;

		sf::Vector2f offset;
		sf::RectangleShape shape;

		void select(
			sf::Vector2i offset,
			sf::Vector2i size)
		{
			Game::Tile::Id lastId = Game::Tile::Id::InvalidTile;

			selection->tiles.clear();
			selection->type = Selection::Type::EqualType;

			for (TileBase* tile : world->getTiles())
				if (tile->getShape()->getPosition().x + tile->getShape()->getSize().x > offset.x &&
					tile->getShape()->getPosition().y + tile->getShape()->getSize().y > offset.y &&

					tile->getShape()->getPosition().x < offset.x + size.x &&
					tile->getShape()->getPosition().y < offset.y + size.y)
				{
					if (selection->type == Selection::Type::EqualType)
					{
						if (lastId == Game::Tile::Id::InvalidTile)
						{
							lastId = tile->getId();
						}
						else
						{
							if (lastId != tile->getId())
							{
								selection->type = Selection::Type::Random;
							}
						}
					}

					selection->tiles.push_back(tile);
				}

			if (selection->tiles.size() == 0)
			{
				const sf::Vector2f 
					c_position1 = view->convertPtoC(sf::Vector2i(
						offset.x + (size.x < 0 ? size.x : 0),
						offset.y + (size.y < 0 ? size.y : 0)
				)), // 2 is always bigger
					c_position2 = view->convertPtoC(sf::Vector2i(
						offset.x + (size.x >= 0 ? size.x : 0),
						offset.y + (size.y >= 0 ? size.y : 0)
				));

				selection->altSelectionRect.setPosition(sf::Vector2f(
					c_position1.x - fmod(c_position1.x, (float) view->getGridSize()) - (c_position1.x < 0 ? (float) view->getGridSize() : 0),
					c_position1.y - fmod(c_position1.y, (float) view->getGridSize()) - (c_position1.y < 0 ? (float) view->getGridSize() : 0)
				));

				const sf::Vector2f c_real_position2 = sf::Vector2f(
					c_position2.x - fmod(c_position2.x, (float)view->getGridSize()) - (c_position2.x < 0 ? (float) view->getGridSize() : 0) + (float) view->getGridSize(),
					c_position2.y - fmod(c_position2.y, (float)view->getGridSize()) - (c_position2.y < 0 ? (float) view->getGridSize() : 0) + (float) view->getGridSize()
				);

				selection->altSelectionRect.setSize(sf::Vector2f(
					c_real_position2.x - selection->altSelectionRect.getPosition().x,
					c_real_position2.y - selection->altSelectionRect.getPosition().y
				));
			}
		}

		sf::RectangleShape selectedTileShape;

		const sf::Color selectedTileColor = 
			sf::Color::Color(100, 100, 100, 60);
		const sf::Color color = 
			sf::Color::Color(150, 150, 150, 100);
	};
}
