#pragma once

#include <Client/source/game/Environment.h>
#include <Client/source/game/tiletrait/GameTileBase.h>

#include <Client/source/game/GameEvent.h>

namespace Game
{
	class TransitiveTile
		:
		public virtual GameTileBase
	{
	public:
		typedef sf::Uint8 Index;

		struct Event
		{
			Resource::WorldId target;
			sf::Vector2f sourceOffset;
		};

		TransitiveTile(const Resource::WorldId target)
			:
			target(target)
		{
		}

		void registerType(Environment* const env) override
		{
			env->registerTile<TransitiveTile>(this);
		}

		GameEvent<TransitiveTile, const Event&> onTransition;

		Resource::WorldId getTarget() const
		{
			return target;
		}

	protected:
		void notifyTransitionEvent(const sf::Vector2f sourceOffset)
		{
			Event event;
			
			event.sourceOffset = sourceOffset;
			event.target = target;

			onTransition.notify(event);
		}

		Resource::WorldId target;
	};
}
