#pragma once

#include <Client/source/game/ControllablePlayer.h>
#include <Client/source/game/net/FrameStatus.h>

namespace Game
{
	class CachedControllablePlayer
		:
		public ControllablePlayer
	{
	public:
		using ControllablePlayer::ControllablePlayer;

		void onInternalUpdate() override
		{
			ControllablePlayer::onInternalUpdate();

			Net::FrameStatus& status = packedFrameStatus.frames.emplace_back();

			status.setKey(Device::GameCoreInputSymbol::Trigger, interactController.getCurrentState());
			status.setKey(Device::GameCoreInputSymbol::Reset, respawnController.getCurrentState());
			status.setKey(Device::GameCoreInputSymbol::Up, upController.getCurrentState());
			status.setKey(Device::GameCoreInputSymbol::Left, leftController.getCurrentState());
			status.setKey(Device::GameCoreInputSymbol::Down, downController.getCurrentState());
			status.setKey(Device::GameCoreInputSymbol::Right, rightController.getCurrentState());
		}

		Net::PackedFrameStatus packedFrameStatus;
	};
}
