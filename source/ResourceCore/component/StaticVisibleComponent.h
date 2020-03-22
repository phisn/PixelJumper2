#pragma once

#include "ResourceCore/ComponentResource.h"
#include "ResourceCore/TrivialResource.h"

#include "SFML/Graphics/Color.hpp"

namespace Resource
{
	struct StaticVisibleComponentContent
	{
		sf::Color color;
	};

	typedef TrivialResource<StaticVisibleComponentContent> StaticVisibleComponent;
}
