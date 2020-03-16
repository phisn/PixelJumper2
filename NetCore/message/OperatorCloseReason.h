#pragma once

namespace Net
{
	struct OperatorCloseReason
	{
		enum
		{
			ConnectionClosed,
			IdleConnection
		};
	};
}