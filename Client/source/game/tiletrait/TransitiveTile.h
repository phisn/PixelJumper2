#pragma once

#include <Client/source/game/Environment.h>
#include <Client/source/game/tiletrait/GameTileBase.h>

#include <Client/source/game/GameEvent.h>

namespace Game
{
	class TransitiveTile
		:
		public RegisterableType
	{
	public:
		TransitiveTile(const Resource::WorldId target)
			:
			target(target)
		{
		}

		void registerType(Environment* const env) override
		{
			env->registerTile<TransitiveTile>(this);
		}

		GameEvent<TransitiveTile, Resource::WorldId> onTransition;

		Resource::WorldId getTarget() const
		{
			return target;
		}

	protected:
		void notifyTransitionEvent()
		{
			onTransition.notify(target);
		}

		Resource::WorldId target;
	};
}
