#pragma once

#include "ComponentResource.h"
#include "TrivialResource.h"

#include "SFML/Graphics/Color.hpp"

namespace Resource
{
	struct StaticVisibleComponentContent
	{
		sf::Color color;
	};

	typedef TrivialResource<StaticVisibleComponentContent> StaticVisibleComponent;
}
