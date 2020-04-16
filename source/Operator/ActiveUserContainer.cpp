#include "ActiveUserContainer.h"

#include <cassert>
#include <unordered_map>

namespace Operator
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
