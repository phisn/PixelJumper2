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

	ClassicHost* FindHost()
	{
		constexpr float optimalUsage = 0.8f;

		if (hosts.size() == 0)
		{
			return NULL;
		}

		decltype(hosts)::const_iterator iterator = hosts.cbegin();

		decltype(hosts)::const_iterator optimalHost = iterator;
		float optimalHostCapacity = iterator->second->getUserCapacityUsage();

		while (++iterator != hosts.cend())
		{
			const float iteratorCapacity = iterator->second->getUserCapacityUsage();

			if (iteratorCapacity < optimalUsage &&
				iteratorCapacity > optimalHostCapacity)
			{
				optimalHost = iterator;
				optimalHostCapacity = iteratorCapacity;
			}
		}

		return optimalHost->second;
	}

	ClassicHost* CreateHost(const ClassicHostConfig& config)
	{
		return hosts[config.host.userID] = new ClassicHost(config);
	}

	void RemoveHost(const UserID userID)
	{
		delete hosts[userID];
		hosts.erase(userID);
	}

	bool ExistsHost(const UserID userID)
	{
		return hosts.find(userID) != hosts.end();
	}
}
