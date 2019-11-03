#include "VirtualPlayer.h"

namespace Game
{
	void VirtualPlayer::handleFrameStatus(const Net::FrameStatus& status)
	{
		if (status.getKey(Device::GameCoreInputSymbol::Left))
		{
			left.call();
		}

		if (status.getKey(Device::GameCoreInputSymbol::Up))
		{
			up.call();
		}

		if (status.getKey(Device::GameCoreInputSymbol::Right))
		{
			right.call();
		}

		if (status.getKey(Device::GameCoreInputSymbol::Down))
		{
			down.call();
		}

		if (status.getKey(Device::GameCoreInputSymbol::Reset))
		{
			respawn.call();
		}

		if (status.getKey(Device::GameCoreInputSymbol::Trigger))
		{
			interact.call();
		}
	}
}