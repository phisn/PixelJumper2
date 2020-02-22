#pragma once

#include <Client/source/game/CollisionEngine.h>
#include <Client/source/resource/WorldResource.h>

#include <cassert>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/VertexBuffer.hpp>

namespace Game
{
	class CollidableTile;
	class Environment
	{
	public:
		typedef std::vector<void*> TileContainer;
		typedef std::vector<CollidableTile*> CollidableTileContainer;

		bool initialize(const Resource::World* const resource);
		bool initializeGraphics()
		{
			return initializeVertex();
		}

		// let hash | valuetype open for ex. diff. collision types
		template <typename TileType>
		const std::vector<TileType*>& getTileType() const
		{
			decltype(tiles)::const_iterator iterator = tiles.find(typeid(TileType));

			if (iterator == tiles.cend())
			{
				return (const std::vector<TileType*>&) emptyVector;
			}

			return (const std::vector<TileType*>&) iterator->second;
		}

		const std::vector<CollidableTile*>& getCollisionTileType(const CollisionType type)
		{
			decltype(collisionTypeTiles)::iterator iterator = collisionTypeTiles.find(type);

			if (iterator == collisionTypeTiles.cend())
			{
				return (const std::vector<CollidableTile*>&) emptyVector;
			}

			return iterator->second;
		}

		template <typename TileType>
		void registerTile(TileType* const tile)
		{
			tiles[typeid(TileType)].push_back((void*) tile);
		}
		
		void processLogic();

		void draw(sf::RenderTarget* const target) const;
		void onEvent(const sf::Event event);
		
		const std::vector<CollisionType>& getCollisionTypes()
		{
			return collisionTypes;
		}

		void registerCollisionType(
			const CollisionType type, 
			CollidableTile* const tile)
		{
			std::vector<CollidableTile*>& tiles = collisionTypeTiles[type];

			if (tiles.size() == 0)
			{
				collisionTypes.push_back(type);
			}

			tiles.push_back(tile);
		}

	private:
		bool initializeCreateAndRegister(const Resource::World* const resource);
		bool initializeVertex();

		std::unordered_map<std::type_index, TileContainer> tiles;
		std::unordered_map<CollisionType, CollidableTileContainer> collisionTypeTiles;

		std::vector<CollisionType> collisionTypes;
		std::vector<void*> emptyVector;

		std::vector<sf::VertexBuffer> vertexWorld;
	};
}
