#pragma once

#include <Client/source/game/net/NetworkMessage.h>

namespace Operator::Net
{
	struct AuthenticationMessageID
	{
		enum
		{
			Register,
			Login
		};
	};
}
