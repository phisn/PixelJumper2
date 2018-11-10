#pragma once

#include <Client/source/editor/manipulator/cache/Cache.h>

namespace Editor
{
	// Represent -> Worker -> Input to Output
	class Element
	{
	public:
		// return == notify components
		virtual bool manipulate(
			Cache::Input* const input,
			Cache::Output* const output,

			const Cache::Sector sector) = 0;
	};
}
