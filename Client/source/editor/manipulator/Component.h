#pragma once

#include <Client/source/editor/manipulator/cache/Cache.h>

namespace Editor
{
	// Represent -> observer
	class Component
	{
	public:
		virtual void notify(
			const Cache::Sector sector) = 0;
	};
}
