#pragma once

#include <Client/source/resource/CommonResourceTypes.h>
#include <Client/source/resource/ResourceBase.h>

#include <Client/source/shared/tiles/TileCommon.h>

#include <SFML/Main.hpp>

namespace Resource
{
	class TileBase
		:
		public ResourceBase
	{
	public:
		TileBase(
			const Shared::TileId id,
			const TileContentSize size)
			:
			id(id),
			contentSize(size)
		{
		}

		Shared::TileId getTileId() const
		{
			return id;
		}

		TileContentSize getContentSize() const
		{
			return contentSize;
		}
		
	private:
		const Shared::TileId id;
		const TileContentSize contentSize;
	};
}
