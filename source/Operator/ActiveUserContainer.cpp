#include "ActiveUserContainer.h"

#include <cassert>
#include <unordered_map>

namespace Operator::Net
{
	std::unordered_map<UserID, UsermodeContainer*> users;

	Usermode GetUserMode(UserID userID)
	{
		decltype(users)::const_iterator user = users.find(userID);
		
		return user == users.cend()
			? Usermode::Offline
			: user->second->usermode;
	}

	UsermodeContainer::UsermodeContainer(UserID userID)
		:
		userID(userID)
	{
		assert(users.find(userID) == users.end());
		users[userID] = this;
	}

	UsermodeContainer::~UsermodeContainer()
	{
		users.erase(userID);
	}
}

namespace Operator::Net::_ClassicHostContainer
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
