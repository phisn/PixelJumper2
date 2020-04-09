#pragma once

#include "Common/Common.h"

#include <steam/steamnetworkingsockets.h>

#include <vector>

namespace Operator
{
	struct HostContainer
	{
		HostContainer(UserID userID);
		~HostContainer();

		SteamNetworkingIPAddr address;
		UserID userID;
	};

	const HostContainer* FindHost(UserID userID);

	class ClassicHostContainer
	{
	public:
		ClassicHostContainer(UserID userID);
		~ClassicHostContainer();

		HostContainer host;
		sf::Uint32 maxPlayers;

		enum class RegisterUserFailure
		{
			Success,
			UserAlreadyHosted,
			MaxUserCount
		};

		RegisterUserFailure registerUser(UserID userID)
		{
			if (users.size() >= maxPlayers)
			{
				return RegisterUserFailure::MaxUserCount;
			}

			if (FindClassicHostFromUser(userID) != NULL)
			{
				return RegisterUserFailure::UserAlreadyHosted;
			}

			users.push_back(userID);
		}

		void unregisterUser(UserID userID)
		{
			decltype(users)::iterator iterator = std::find(
				users.begin(),
				users.end(), userID);

			if (iterator != users.end())
				users.erase(iterator);
		}

		float getCapacityUsageDifference() const
		{
			constexpr float optimalCapacity = 0.5;
			return fabsf(getCapactiyUsage() - optimalCapacity);
		}

		float getCapactiyUsage() const
		{
			return (float) users.size() / (float) maxPlayers;
		}

		const std::vector<UserID> getUsers() const
		{
			return users;
		}

	private:
		std::vector<UserID> users;
	};

	const ClassicHostContainer* FindClassicHost(UserID userID);
	const ClassicHostContainer* FindOptimalClassicHost();
	// be aware O(n^2)
	const ClassicHostContainer* FindClassicHostFromUser(UserID userID);
}
