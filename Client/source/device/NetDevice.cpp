#include "NetDevice.h"

namespace Device::Net
{
	bool Initialize()
	{
		SteamDatagramErrMsg error;
		if (!GameNetworkingSockets_Init(NULL, error))
		{
			Log::Error(L"Failed to initialize (", carrtowstr(error), L")");
			return false;
		}

		SteamNetworkingUtils()->SetDebugOutputFunction(
			k_ESteamNetworkingSocketsDebugOutputType_Msg,
			[](const ESteamNetworkingSocketsDebugOutputType type,
				const char* const message)
			{
				if (type == ESteamNetworkingSocketsDebugOutputType::k_ESteamNetworkingSocketsDebugOutputType_Msg ||
					type == ESteamNetworkingSocketsDebugOutputType::k_ESteamNetworkingSocketsDebugOutputType_Important)
				{
					Log::Information(L"NetworkUtils message (", carrtowstr(message), L")");
					return;
				}

				if (type >= ESteamNetworkingSocketsDebugOutputType::k_ESteamNetworkingSocketsDebugOutputType_Warning)
				{
					Log::Information(L"NetworkUtils warning (", carrtowstr(message), L")");
					return;
				}

				Log::Error(L"NetworkUtils different (", carrtowstr(message), L")",
					(int)type, L"type");
			}
		);

		return true;
	}

	void Uninitialize()
	{
		GameNetworkingSockets_Kill();
	}
}
