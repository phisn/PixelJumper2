#pragma once

#include "Common/Notifier.h"

namespace Editor
{
	class ClassicWorld
	{
	public:
		std::string name;
	};
	
	class ClassicStage
	{

	};

	class ClassicContext
	{
	public:
		std::vector<ClassicWorld*> worlds;
	};
}
