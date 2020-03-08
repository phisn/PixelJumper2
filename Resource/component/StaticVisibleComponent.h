#pragma once

#include "Resource/ComponentResource.h"
#include "Resource/TrivialResource.h"

#include "SFML/Graphics/Color.hpp"

namespace Resource
{
	struct StaticVisibleComponentContent
	{
		sf::Color color;
	};

	typedef TrivialResource<StaticVisibleComponentContent> StaticVisibleComponent;
}
