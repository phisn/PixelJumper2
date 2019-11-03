#pragma once

#include <Client/source/game/GamePlayer.h>
#include <Client/source/game/GameWorld.h>

#include <Client/source/game/net/FrameStatus.h>

namespace Game
{
	class VirtualPlayer
		:
		public PlayerBase
	{
	public:
		VirtualPlayer(const PlayerInformation information)
			:
			PlayerBase(information)
		{
		}

		void pushEmptyUpdate()
		{
			pushUpdate(Net::FrameStatus{ });
		}

		void pushUpdate(const Net::FrameStatus status)
		{
			statusDeque.push_back(status);
		}

		bool hasUpdate()
		{
			return !statusDeque.empty();
		}

		void onInternalUpdate() override
		{
			assert(hasUpdate());

			handleFrameStatus(statusDeque.front());
			statusDeque.pop_front();

			PlayerBase::onInternalUpdate();
		}

	private:
		void handleFrameStatus(const Net::FrameStatus& status);
		std::deque<Net::FrameStatus> statusDeque;
	};
}
