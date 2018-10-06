#pragma once

#include <Client/source/game/LocalPlayer.h>
#include <Client/source/game/World.h>

#include <vector>

#ifndef GAME
#define GAME ::Game
#endif

#define IS_STOPPABLE(type) (type <= GAME::Type::Global)
#define IS_ONLINE(type) (type < GAME::Type::Global)

namespace Game
{
	enum class Type
	{
		LocalArcade,
		LocalCoop,
		LocalCompetitive,
		Global, // stoppable

		OnlineArcade,
		OnlineCoop,
		OnlineCompetitive
	};

	namespace Interface
	{
		bool initialize();
		void reset();

		void setType(Type type);
		const Type getType();

		const World* getWorld();
		const std::vector<LocalPlayer*> getLocalPlayers();
	};
}