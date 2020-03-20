#include "VirtualPlayer.h"

namespace Game
{
	void VirtualPlayer::handleFrameStatus(const FrameStatus& status)
	{
		if (status.getKey(InputSymbol::Left))
		{
			left.call();
		}

		if (status.getKey(InputSymbol::Up))
		{
			up.call();
		}

		if (status.getKey(InputSymbol::Right))
		{
			right.call();
		}

		if (status.getKey(InputSymbol::Down))
		{
			down.call();
		}

		if (status.getKey(InputSymbol::Reset))
		{
			respawn.call();
		}

		if (status.getKey(InputSymbol::Trigger))
		{
			interact.call();
		}
	}
}