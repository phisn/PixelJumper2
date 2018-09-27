#pragma once

#include "Player.h"

#ifndef PJ
#define PJ ::PixelJumper
#endif

namespace PixelJumper
{
	class RemotePlayer
	{
	public:
		virtual void onRemoteUpdate() = 0;
	};
}
