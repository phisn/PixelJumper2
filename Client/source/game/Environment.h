#pragma once

#include <Client/source/game/CollisionEngine.h>
#include <Client/source/game/tiletrait/GameTileBase.h>

#include <Client/source/resource/WorldResource.h>

#include <cassert>
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
		bool initialize(const Resource::World* const resource);
		bool initializeGraphics()
		{
			return initializeVertex();
		}

		// let hash | valuetype open for ex. diff. collision types
		template <typename HashType, typename ValueType = HashType>
		const std::vector<ValueType*>& getTileType() const
		{
			decltype(tiles)::const_iterator iterator = tiles.find(typeid(HashType).hash_code());

			if (iterator == tiles.cend())
			{
				return (const std::vector<ValueType*>&) emptyVector;
			}

			return (const std::vector<ValueType*>&) iterator->second;
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

		template <typename HashType, typename ValueType = HashType>
		void registerTile(ValueType* const tile)
		{
			tiles[typeid(HashType).hash_code()].push_back((GameTileBase*) tile);
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

		std::unordered_map<
			size_t,
			std::vector<GameTileBase*>
		> tiles;

		std::unordered_map<
			CollisionType,
			std::vector<CollidableTile*>
		> collisionTypeTiles;

		std::vector<CollisionType> collisionTypes;
		std::vector<GameTileBase*> emptyVector;

		std::vector<sf::VertexBuffer> vertexWorld;
	};
}
