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
		Environment()
		{
		}

		// let hash | valuetype open for ex. diff. collision types
		template <typename HashType, typename ValueType = HashType>
		const std::vector<ValueType*>& getTileType() const
		{
			return (const std::vector<ValueType*>&) tiles[typeid(HashType).hash_code()];
		}

		const std::vector<CollidableTile*>& getCollisionTileType(const CollisionType type)
		{
			decltype(collisionTypeTiles)::iterator result = collisionTypeTiles.find(type);
			assert(result != collisionTypeTiles.cend());
			return result->second;
		}

		template <typename HashType, typename ValueType = HashType>
		void registerTile(ValueType* const tile)
		{
			tiles[typeid(HashType).hash_code()].push_back(tile);
		}

		bool initialize(
			Resource::World* const resource);

		void draw();
		void onEvent(const sf::Event event);
		void onLogic(const sf::Time time);

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
		std::unordered_map<
			size_t,
			std::vector<GameTileBase*>
		> tiles;

		std::unordered_map<
			CollisionType,
			std::vector<CollidableTile*>
		> collisionTypeTiles;

		std::vector<CollisionType> collisionTypes;

		std::vector<
			sf::VertexBuffer
		> vertexWorld;
	};
}
