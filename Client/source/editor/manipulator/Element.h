#pragma once

#include <Client/source/editor/manipulator/Cache.h>

namespace Editor
{
	// Represent -> Worker -> Input to Output
	class Element
	{
	public:
		virtual void manipulate(
			Cache::Input* const input,
			Cache::Output* const output,

			const Cache::Sector sector) = 0;
	};
}
