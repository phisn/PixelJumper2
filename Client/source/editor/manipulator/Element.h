#pragma once

#include <Client/source/editor/manipulator/CacheManager.h>
#include <Client/source/editor/manipulator/Manipulator.h>

namespace Editor
{
	class Element
	{
	protected:
		void notifyExecutor()
		{
			Manipulator::getChaceManager()->notify();
		}
	};
}
