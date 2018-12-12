#pragma once

#include <SFML/Main.hpp>

#define DEFAULT_RES_PATH L"resource/"
#define DEFAULT_RES_PATH_APPEND(resource) DEFAULT_RES_PATH #resource
#define DEFAULT_RES_EXTENSION L"pxjmp"

namespace Resource
{
	typedef sf::Uint64 Magic;

	enum class ResourceType
	{
		World,

		_Length
	};
}
