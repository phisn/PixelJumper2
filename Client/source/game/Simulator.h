#pragma once

#include <Client/source/game/PlayerState.h>
#include <Client/source/game/CollisionEngine.h>

namespace Game
{
	namespace PlayerEvent
	{
		struct LeaveEvent
		{
			CollisionEngine::CollisionInfo collisionInfo;
			PlayerState* playerState;
		};

		struct EnterEvent
		{
			CollisionEngine::CollisionInfo collisionInfo;
			PlayerState* playerState;
		};

		// entry point defines default restart handler
		typedef void (*RestartHandler)(PlayerState* const);
		typedef void (*FinishHandler)(PlayerState* const);
	}

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

	struct Collision
	{
		enum Type {
			Horizontal,
			Vertical
		} type;

		bool isPositive; // right = position; left = negative = !positive

		PlayerState* const player; // contains collision source
		const sf::Vector2f destination;
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
