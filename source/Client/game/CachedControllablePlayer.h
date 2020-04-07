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
			if (injectedFrames.size() > 0)
			{
				// injected frames do not count to packedFrameStatus
				handleInjectedFrame(injectedFrames.front());
				injectedFrames.pop_front();

				LocalPlayer::onInternalUpdate();
			}
			else
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
		}

		void inject(FrameStatus frame)
		{
			injectedFrames.push_back(frame);
		}

		PackedFrameStatus packedFrameStatus;
		std::deque<FrameStatus> injectedFrames;

	private:
		void handleInjectedFrame(FrameStatus frame);
	};
}
