#pragma once

#include "ControllablePlayer.h"

#include "GameCore/net/FrameStatus.h"

namespace Game
{
	class CachedControllablePlayer
		:
		public ControllablePlayer
	{
	public:
		CachedControllablePlayer(
			const int playerID,
			const PlayerInformation information,
			Device::View* const view)
			:
			ControllablePlayer(playerID, information, view)
		{
		}

		void onInternalUpdate() override
		{
			ControllablePlayer::onInternalUpdate();

			Game::FrameStatus& status = packedFrameStatus.frames.emplace_back();

			status.setKey(Game::InputSymbol::Trigger, interactController.getCurrentState());
			status.setKey(Game::InputSymbol::Reset, respawnController.getCurrentState());

			status.setKey(Game::InputSymbol::Up, upController.getCurrentState());
			status.setKey(Game::InputSymbol::Left, leftController.getCurrentState());
			status.setKey(Game::InputSymbol::Down, downController.getCurrentState());
			status.setKey(Game::InputSymbol::Right, rightController.getCurrentState());
		}

		Game::PackedFrameStatus packedFrameStatus;
	};
}
