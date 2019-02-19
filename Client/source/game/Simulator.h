#pragma once

#include <Client/source/game/PlayerState.h>
#include <Client/source/game/CollisionEngine.h>

namespace Game
{
	enum class TileEventType // dependent from tile
	{
		Acceleration,
		Jump,
		Collision
	};

	enum class PlayerEventType // independent from tile
	{
		Restart,
		Finish
	};
	
	namespace Simulator
	{
		struct Settings
		{
			/*
			
				Problem:
					Coop mode does need shared respawn point.
					Respawn is realized though PlayerEvent hooks.
					After a single player has reached the checkpoint,
					every player should be able to respawn there.
				Solution:
					Let the gamemode decide to use Global PlayerEvents
					or Single PlayerEvents
			
			*/

			bool useGlobalPlayerEventHook;
		};

		bool Initialize(const Settings settings);
		void Uninitialize();

		void HookPlayerEvent(
			PlayerState* const player,
			const PlayerEventType eventType);
		void HookSinglePlayerEvent(
			PlayerState* const player,
			const PlayerEventType eventType);
		void HookGlobalPlayerEvent(
			const PlayerEventType eventType);

	}
}
