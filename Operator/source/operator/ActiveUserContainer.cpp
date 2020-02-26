#include "ActiveUserContainer.h"

#include <unordered_map>

namespace Operator::Net::UserContainer
{
	namespace
	{
		std::unordered_map<UserID, ActiveUserMode> users;
	}

	void SetUserMode(const UserID userID, const ActiveUserMode mode)
	{
		users[userID] = mode;
	}

	ActiveUserMode GetUserMode(const UserID userID)
	{
		return users[userID];
	}
}

namespace Operator::Net::ClassicHostContainer
{
	namespace
	{
		std::unordered_map<UserID, ClassicHost*> hosts;
	}

	UserID FindHost()
	{
		constexpr float optimalUsage = 0.8f;

		if (hosts.size() == 0)
		{
			return NULL;
		}

		decltype(hosts)::const_iterator iterator = hosts.cbegin();

		UserID optimalHost = iterator->first;
		float optimalHostCapacity = iterator->second->getUserCapacityUsage();

		while (++iterator != hosts.cend())
		{
			const float iteratorCapacity = iterator->second->getUserCapacityUsage();

			if (iteratorCapacity < optimalUsage &&
				iteratorCapacity > optimalHostCapacity)
			{
				optimalHost = iterator->first;
				optimalHostCapacity = iteratorCapacity;
			}
		}

		return optimalHost;
	}

	ClassicHost* CreateHost(const ClassicHostConfig& config)
	{
		return hosts[config.host.userID] = new ClassicHost(config);
	}

	void RemoveHost(const UserID userID)
	{
		hosts.erase(userID);
	}

	bool ExistsHost(const UserID userID)
	{
		return hosts.find(userID) != hosts.end();
	}
}
