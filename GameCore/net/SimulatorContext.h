#pragma once

#include "NetPlayerMovement.h"

#include "Resource/PlayerResource.h"
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
	
	struct LocationPlayer;
	struct SimulatorContextLocationCallback
	{
		virtual void onNetPlayerMovementPush(NetPlayerMovement* const movement) = 0;
		virtual void onNetPlayerAdded(LocationPlayer* const locationPlayer) = 0;
		virtual void onNetPlayerRemoved(const Resource::PlayerID playerID) = 0;
	};

	struct LocationPlayer
	{
		SimulatorContextLocationCallback* callback;
		Resource::PlayerID playerID;
		Resource::RepresentationID representationID;
	};

	struct Location
	{
		std::vector<LocationPlayer*> players;

		void removePlayer(const Resource::PlayerID playerID)
		{
			players.erase(
				std::find_if(
					players.begin(),
					players.end(),
					[playerID](LocationPlayer* const player)
					{
						return player->playerID == playerID;
					})
			);
		}

		void pushMovement(NetPlayerMovement* const movement)
		{
			for (LocationPlayer* const player : players)
				if (player->playerID != movement->content.playerID)
				{
					player->callback->onNetPlayerMovementPush(movement);
				}

			movement->positions.clear();
		}
	};

	// TODO: need to add the possiblity to get players by world
	// to allow the simulation to show other players ghost in the
	// same world
	class SimulatorContext
	{
	public:
		Location* putPlayer(
			LocationPlayer* const player,
			const Resource::WorldId worldID)
		{
			Location* const location = &locationPlayers[worldID];

			for (LocationPlayer* const locationPlayer : location->players)
				locationPlayer->callback->onNetPlayerAdded(player);
			
			location->players.push_back(player);
			
			return location;
		}

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
		std::map<Resource::WorldId, Location> locationPlayers;

		// callbacks are parallel to players. not implemented
		// as pair to allow easy return of players container without
		// exposing callbacks
		std::vector<SimulatorContextCallback*> callbacks;
		std::vector<Resource::PlayerResource*> players;
	};
}
