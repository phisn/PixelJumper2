#include "ActiveHostContainer.h"

#include <cassert>
#include <unordered_map>

namespace Operator
{
	std::unordered_map<UserID, HostContainer*> hosts;
	std::unordered_map<UserID, ClassicHostContainer*> classicHosts;

	HostContainer::HostContainer(UserID userID)
		:
		userID(userID)
	{
		assert(hosts.find(userID) == hosts.end());
		hosts[userID] = this;
	}

	HostContainer::~HostContainer()
	{
		assert(hosts.find(userID) != hosts.end());
		hosts.erase(userID);
	}

	const HostContainer* FindHost(UserID userID)
	{
		decltype(hosts)::iterator host = hosts.find(userID);
		return host == hosts.end()
			? NULL
			: host->second;
	}

	ClassicHostContainer::ClassicHostContainer(UserID userID)
		:
		host(userID)
	{
		classicHosts[userID] = this;
	}

	ClassicHostContainer::~ClassicHostContainer()
	{
		classicHosts.erase(host.userID);
	}

	const ClassicHostContainer* FindClassicHost(UserID userID)
	{
		decltype(classicHosts)::iterator host = classicHosts.find(userID);
		return host == classicHosts.end()
			? NULL
			: host->second;
	}

	const ClassicHostContainer* FindOptimalClassicHost()
	{
		constexpr float optimalUsage = 0.5f;

		if (classicHosts.size() == 0)
		{
			return NULL;
		}

		decltype(classicHosts)::iterator hostIterator = classicHosts.begin();
		decltype(classicHosts)::iterator optimalHost = classicHosts.begin();

		float optimalHostCapacityDifference = optimalHost->second->getCapacityUsageDifference();

		while (++hostIterator != classicHosts.cend())
		{
			float hostCapcityDifference = hostIterator->second->getCapacityUsageDifference();

			if (optimalHostCapacityDifference > hostCapcityDifference)
			{
				optimalHost = hostIterator;
				optimalHostCapacityDifference = hostCapcityDifference;
			}
		}

		return optimalHost->second;
	}

	const ClassicHostContainer* FindClassicHostFromUser(UserID userID)
	{
		for (decltype(classicHosts)::value_type& host : classicHosts)
			for (UserID iterator_userID : host.second->users)
				if (iterator_userID == userID)
				{
					return host.second;
				}

		return NULL;
	}
}
