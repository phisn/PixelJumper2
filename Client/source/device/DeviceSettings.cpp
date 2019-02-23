#include "DeviceSettings.h"

#include <Client/source/resource/ResourceInterface.h>

// goal -> "./resource/settings/{...}"

namespace
{
	std::wstring settingsPath;

	std::filesystem::path screenResourcePath;
}

namespace Device
{
	bool Settings::Initialize()
	{
	}

	void Settings::Uninitialize()
	{
	}

	bool Settings::LoadScreenSettings(
		ScreenSettings* const settings)
	{
		return Resource::Interface::ReadRawResource(settings, screenResourcePath);
	}

	bool Settings::SaveScreenSettings(
		ScreenSettings* const settings)
	{
		return Resource::Interface::WriteRawResource(settings, screenResourcePath);
	}

	bool Settings::LoadGeneralInputSettings(GeneralInputSettings * const settings)
	{
		return false;
	}

	bool Settings::SaveGeneralInputSettings(const GeneralInputSettings * const settings)
	{
		return false;
	}

	bool Settings::LoadGameInputSettings(GameInputSettings * const settings, const int playerNumber)
	{
		return false;
	}

	bool Settings::SaveGameInputSettings(const GameInputSettings * const settings, const int playerNumber)
	{
		return false;
	}
}
