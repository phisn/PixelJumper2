#pragma once

#include "Resource/ComponentResource.h"
#include "Resource/TrivialResource.h"

namespace Resource
{
	struct TileComponentContent
	{
		sf::Vector2<sf::Uint16> position;
		sf::Vector2<sf::Uint16> size;
	};

	typedef TrivialResource<TileComponentContent> TileComponent;
}
