#pragma once

#include <SFML/System/Vector2.hpp>

namespace Game
{
	class TileContainer;
	class GameTileBase
	{
	public:
		/*
			
			initialize should call insert methods of tilecontainer
			in upper classes

			-- Example --

			TileContainer.initialize 
			-> GameTileBase.initialize 
			--> WallTile.initialize 
			---> CollidableTile.initialize 
			----> TileContainer.insertCollidable
			---> StaticTile.initialize
			----> TileContainer.insertStatic

			-------------
		*/
		virtual void initialize(TileContainer* const) = 0;
		virtual const sf::Vector2f getPosition() const = 0;
		virtual const sf::Vector2f getSize() const = 0;
	};
}
