#pragma once

#include <client/source/editor/manipulator/cache/ModeCache.h>
#include <Client/source/editor/manipulator/cache/SelectionCache.h>
#include <client/source/editor/manipulator/cache/TileCache.h>

#include <Client/source/editor/template/TileTemplate.h>

#include <SFML/Graphics.hpp>

#include <vector>

namespace Editor
{
	class CacheManager;
	class Cache
	{
		friend CacheManager;
	public:
		/*	
			To change many you have
			to call repeatedly

			Propably same as Input

		*/
		enum class Sector
		{
			_Invalid,

			Mode,
			Selection,
			Tile
		};

		// modifyable by everyone
		struct Input
		{
			ModeCache::Input mode;
			SelectionCache::Input selection;
			TileCache::Input tile;

		};

		// modifyable though manager
		struct Output
		{
			SelectionCache::Output selection;

		};

		Input* writeInput()
		{
			return &input;
		}

		// relevant for Executor and Components
		const Output* readOutput() const
		{
			return &output;
		}

	private:
		Input input;
		Output output;
	};
}