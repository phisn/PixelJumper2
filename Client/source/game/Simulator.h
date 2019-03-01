#pragma once

#include <Client/source/game/PlayerState.h>
#include <Client/source/game/CollisionEngine.h>

namespace Game
{
	template <typename Target, typename Type>
	class ExternTargetProperty
	{
	public:
		typedef std::function<void(Target* const, const Type)> Listener;

		void addListener(
			const Listener listener)
		{
			listeners.insert(listener);
		}

		void removeListener(
			const Listener listener)
		{
			listeners.erase(listener);
		}

		void callListeners(
			Target* const target,
			const Type oldValue) const
		{
			for (const Listener listener : listeners)
			{
				listener(target, oldValue);
			}
		}

	private:
		std::unordered_set<Listener> listeners;
	};

	template <typename Type>
	using PlayerProperty = ExternTargetProperty<PlayerState, Type>;
	template <typename Type>
	using WorldProperty = ExternTargetProperty<WorldState, Type>;

	template <typename Target, typename... Args>
	class RoutineContainer
	{
	public:
		typedef std::function<void(Target* const, Args...)> Routine;

		RoutineContainer(
			const Routine defaultRoutine)
			:
			defaultRoutine(defaultRoutine),
			currentRoutine(defaultRoutine)
		{
		}

		Routine GetDefault() const
		{
			return defaultRoutine;
		}

		void Hook(const Routine routine)
		{
			currentRoutine = routine;
		}

		void Unhook()
		{
			Hook(defaultRoutine);
		}

		void CallRoutine(
			Target* const target,
			Args... args)
		{
			currentRoutine(target, args...);
		}

	private:
		const Routine defaultRoutine;
		Routine currentRoutine;
	};

	template <typename... Args>
	using PlayerRoutineContainer = RoutineContainer<PlayerState, Args...>;

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

		namespace Player
		{
			PlayerProperty<sf::Vector2f>* GetPositionProperty();
			PlayerProperty<sf::Vector2f>* GetMovementProperty();

			PlayerRoutineContainer<Direction>* GetMovementRoutineContainer();
			PlayerRoutineContainer<>* GetTriggerRoutineContainer();
		}

		namespace World
		{
			WorldProperty<sf::Uint16> GetPlayerCountProperty();
		}
	}
}
