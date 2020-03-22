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
		
		std::deque<FrameStatus> statusDeque;

	private:
		void handleFrameStatus(const FrameStatus& status);

	};
}
