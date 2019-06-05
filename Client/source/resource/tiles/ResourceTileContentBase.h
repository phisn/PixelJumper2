#pragma once

#include <Client/source/game/tile/TileId.h>
#include <Client/source/resource/CommonResourceTypes.h>
#include <Client/source/resource/ResourceBase.h>

#include <SFML/Main.hpp>

namespace Resource
{
	class TileBase
		:
		public ResourceBase
	{
	public:
		TileBase(
			const Game::TileId id,
			const TileContentSize size)
			:
			id(id),
			contentSize(size)
		{
		}

		Game::TileId getTileId() const
		{
			return id;
		}

		TileContentSize getContentSize() const
		{
			return contentSize;
		}
		
	private:
		const Game::TileId id;
		const TileContentSize contentSize;
	};
}
