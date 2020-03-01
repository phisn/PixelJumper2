#pragma once

#include <Client/source/resource/PlayerResource.h>

#include <vector>

namespace Game::Net
{
	// callback used by clienthandlers
	struct SimulatorContextCallback
	{
		// passed as nonconst for compatability
		// when called player is yet not inserted in context
		virtual void playerRegistered(Resource::PlayerResource* const player) = 0;

		// called after player is already removed from context
		// removed player callback is not called
		virtual void playerUnregistered(const Operator::UserID userID) = 0;
	};

	// TODO: need to add the possiblity to get players by world
	// to allow the simulation to show other players ghost in the
	// same world
	class SimulatorContext
	{
	public:
		void registerPlayer(
			Resource::PlayerResource* const player,
			SimulatorContextCallback* const callback)
		{
			for (SimulatorContextCallback* const _callback : callbacks)
				_callback->playerRegistered(player);

			callbacks.push_back(callback);
			players.push_back(player);
		}

		// should only be called by the clienthandler itself
		void unregisterPlayer(const Operator::UserID userID)
		{
			decltype(players)::iterator iterator = players.begin();

			while (iterator != players.end())
				if ((*iterator)->content.playerID == userID)
				{
					players.erase(iterator);

					for (SimulatorContextCallback* const callback : callbacks)
						callback->playerUnregistered(userID);

					break;
				}
				else
				{
					++iterator;
				}
		}

		const std::vector<Resource::PlayerResource*>& getActivePlayers() const
		{
			return players;
		}
		
	private:
		// callbacks are parallel to players. not implemented
		// as pair to allow easy return of players container without
		// exposing callbacks
		std::vector<SimulatorContextCallback*> callbacks;
		std::vector<Resource::PlayerResource*> players;
	};
}
