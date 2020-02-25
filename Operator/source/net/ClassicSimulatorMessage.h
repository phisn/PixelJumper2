#pragma once

// the client for classicsimulatior are trusted
// classic simulators and never normal users
namespace Operator::Net::Client
{
	struct ClassicHostID
	{
		enum
		{
			RequestClientData,

			UnlockRepresentation,
			UnlockWorld
		};
	};
}

namespace Operator::Net::Host
{

}
