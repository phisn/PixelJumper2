#pragma once

namespace Editor
{
	class CacheManager;
	class Cache
	{
		friend CacheManager;
	public:
		// modifyable by everyone
		struct Input
		{
			struct Selection
			{
				sf::Vector2f
					real_size,
					real_offset;
				sf::Vector2i
					tile_size,
					tile_offset;
			};

			TileTemplate* tile;
		};

		// modifyable by executor
		struct Output
		{
			struct Selection
			{
				// general == area selected 
				//   instead of specific tiles
				bool usingGeneralSelection;

				sf::Vector2f
					real_size,
					real_offset;
				sf::Vector2i
					tile_size,
					tile_offset;

				std::vector<TileBase*> tiles;
			};

			TileTemplate* tile;
		};

		Input* writeInput()
		{
			return &input;
		}

		const Output* readOutput() const
		{
			return &output;
		}

	private:
		Input input;
		Output output;
	};
}