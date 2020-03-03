#pragma once

#include <Operator/source/Common.h>

#include <steam/steamnetworkingsockets.h>
#include <vector>

namespace Operator::Net
{
	enum class ActiveUserMode
	{
		Waiting,
		Classic,
		Hosting
	};

	// current user activities are stored here
	// to either find users or prevent joining
	// multiple server simulatnisly

	// all useractivities are reset when the
	// operator is restarted
	namespace UserContainer
	{
		void SetUserMode(
			const UserID userID,
			const ActiveUserMode mode);
		ActiveUserMode GetUserMode(const UserID userID);
	}

	struct HostContent
	{
		SteamNetworkingIPAddr address;
		UserID userID;
	};

	// normal hosts are not connected to the
	// operator and are therefore not gurateed
	// to be valid
	namespace HostContainer
	{

	}

	struct ClassicHostConfig
	{
		HostContent host;
		int maxPlayers;
	};

	class ClassicHost
	{
	public:
		ClassicHost(const ClassicHostConfig& config)
			:
			config(config)
		{
		}

		enum class RegisterUserFailure
		{
			Success,
			InvalidUsermode,
			MaxUserCount
		};

		RegisterUserFailure registerUser(const UserID userID)
		{
			if (users.size() >= config.maxPlayers)
			{
				return RegisterUserFailure::MaxUserCount;
			}

			if (UserContainer::GetUserMode(userID) != ActiveUserMode::Waiting)
			{
				return RegisterUserFailure::InvalidUsermode;
			}

			UserContainer::SetUserMode(
				userID,
				ActiveUserMode::Classic);

			users.push_back(userID);
		}

		void unregisterUser(const UserID userID)
		{
			decltype(users)::iterator iterator = std::find(
				users.begin(),
				users.end(), userID);

			if (iterator != users.end())
			{
				UserContainer::SetUserMode(
					userID,
					ActiveUserMode::Waiting);
				users.erase(iterator);
			}
		}

		int getUserCount() const
		{
			return users.size();
		}

		float getUserCapacityUsage() const
		{
			return (float)users.size() / (float)config.maxPlayers;
		}

		const ClassicHostConfig& getConfig() const
		{
			return config;
		}

	private:
		const ClassicHostConfig config;

		std::vector<UserID> users;
	};

	// classic hosts are always connected to the
	// operator and the stored hosts are guranteed
	// to be valid
	namespace ClassicHostContainer
	{
		ClassicHost* FindHost();
		ClassicHost* CreateHost(const ClassicHostConfig& config);

		void RemoveHost(const UserID userID);
		bool ExistsHost(const UserID userID);
	}
}
