#pragma once

#include "Common/Common.h"

#include <steam/steamnetworkingsockets.h>
#include <optional>
#include <vector>

namespace Operator
{
	enum class Usermode
	{
		Offline,
		Online, 
		Hosting

	};

	Usermode GetUserMode(UserID userID);

	// used to change usermode
	// after releasing object usermode is
	// automatically restored to offline
	class UsermodeContainer
	{
	public:
		UsermodeContainer(UserID userID);
		~UsermodeContainer();

		Usermode usermode;

	private:
		UserID userID;
	};
}
