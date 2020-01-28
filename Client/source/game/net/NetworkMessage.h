#pragma once

#include <Client/source/resource/pipes/PipeBase.h>

namespace Game::Net
{
	struct NetworkMessage
	{
		virtual bool load(Resource::ReadPipe* const pipe) = 0;
		virtual bool save(Resource::WritePipe* const pipe) = 0;
	};
}
