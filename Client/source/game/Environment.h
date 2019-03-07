#pragma once

#include <Client/source/game/tiletrait/GameTileBase.h>
#include <Client/source/resource/WorldResource.h>

#include <typeinfo>
#include <unordered_map>
#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/VertexBuffer.hpp>

namespace Game
{
	class Environment
	{
	public:
		Environment()
		{
		}

		// let hash | valuetype open for ex. diff. collision types
		template <typename HashType, typename ValueType = HashType>
		const std::vector<ValueType*>& getTileType()
		{
			return (const std::vector<ValueType*>&) tiles[typeid(HashType).hash_code()];
		}

		template <typename T>
		void insertTile(T* const tile)
		{
			tiles[typeid(T).hash_code()].push_back(tile);
		}

		bool initialize(
			Resource::World* const resource);

		void draw();
		void onEvent(
			const sf::Event event);
		void onLogic(
			const sf::Time time);

	private:
		std::unordered_map<
			size_t,
			std::vector<GameTileBase*>
		> tiles;

		std::vector<
			sf::VertexBuffer
		> vertexWorld;
	};
}
