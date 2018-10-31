#pragma once

#include <SFML/Graphics.hpp>

#include <Client/source/device/DeviceInterface.h>
#include <Client/source/device/ScreenDevice.h>

#include <Client/source/editor/GridView.h>
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
			const GridView* view,
			World* world)
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

		const GridView* view;
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
				sf::Vector2f c_position = view->convertPtoC(sf::Vector2i(
					size.x < 0 ? offset.x + size.x : offset.x,
					size.y < 0 ? offset.y + size.y : offset.y
				));
				
				if (size.x < -100)
				{
					printf("Hallo");
				}

				sf::Vector2f c_size = view->convertPtoC(sf::Vector2i(
					size.x < 0 ? size.x : -size.x,
					size.y < 0 ? size.y : -size.y
				));

				selection->altSelectionRect.setPosition(sf::Vector2f(
					c_position.x - fmod(c_position.x, (float) view->getGridSize()),
					c_position.y - fmod(c_position.y, (float) view->getGridSize())
				));

				const float x_size = fmod(c_size.x, (float) view->getGridSize());
				const float y_size = fmod(c_size.y, (float) view->getGridSize());

				selection->altSelectionRect.setSize(sf::Vector2f(
					(c_size.x - x_size) + (x_size == 0.f ? 0 : (float) view->getGridSize()),
					(c_size.y - y_size) + (y_size == 0.f ? 0 : (float) view->getGridSize())
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
