#pragma once

#include <Client/source/menu/MenuProperty.h>
#include <Client/source/resource/pipes/PipeBase.h>

#include <cassert>
#include <functional>
#include <unordered_set>

namespace Game
{
	struct GameState
	{
		virtual bool writeState(Resource::WritePipe* const writePipe) = 0;
		virtual bool readState(Resource::ReadPipe* const readPipe) = 0;
	};

	/*template <typename T>
	using Property = Menu::Property<T>;*/
}
