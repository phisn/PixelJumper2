#pragma once

#include <vector>

#include "WorldResource.h"

namespace Resource
{
	// does not conain actual world
	class ClassicWorldResource
	{
	public:
		WorldID worldID;
		std::vector<WorldID> nextWorlds;
	};

	class ClassicStageResource
	{
	public:
		std::wstring name;
		std::vector<ClassicWorldResource> worlds;
	};

	class ClassicContextResource
	{
	public:
		
	};
}
