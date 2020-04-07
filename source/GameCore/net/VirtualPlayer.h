#pragma once

#include "GameCore/GamePlayer.h"
#include "GameCore/GameWorld.h"

#include "FrameStatus.h"

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
			pushUpdate(FrameStatus{ });
		}

		void pushUpdate(const FrameStatus status)
		{
			statusDeque.push_back(status);
		}

		void clearFrames()
		{
			statusDeque.clear();
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

		size_t getFrameCount() const
		{
			return statusDeque.size();
		}

	private:
		std::deque<FrameStatus> statusDeque;
		void handleFrameStatus(const FrameStatus& status);
	};
}
