#include "DeviceConfig.h"

#include <Client/source/resource/ResourceInterface.h>

#include <Client/source/logger/Logger.h>

#define CHECK_TRANSLATION_POSITION(name, position) \
	static_assert((int) Device::ConfigType::##name == position, \
		"ConfigType '" #name "' on wrong position")

namespace
{
	const wchar_t* configTypeTranslations[] =
	{
		L"input",
		L"screen"
	};

	CHECK_TRANSLATION_POSITION(Input, 0);
	CHECK_TRANSLATION_POSITION(Screen, 1);
}

namespace Device
{
	bool VerifySettings(const ConfigType type)
	{
		return true;
	}

	bool Config::Initialize()
	{
		Log::Section section(L"Initializing config");

		for (int i = 0; i < (int) ConfigType::_Length; ++i)
			if (!VerifySettings((ConfigType) i))
			{
				return false;
			}

		return true;
	}

	void Config::Uninitialize()
	{
	}

	bool Config::LoadType(
		Resource::ResourceBase* const resource,
		const ConfigType type)
	{
		if (!VerifySettings(type)) // is this good? (remove)
		{
			return false;
		}

		return Resource::Interface::LoadSettings(
			resource,
			TranslateConfigType(type));
	}

	bool Config::SaveType(
		Resource::ResourceBase* const resource, 
		const ConfigType type)
	{
		return Resource::Interface::SaveSettings(
			resource,
			TranslateConfigType(type));
	}

	bool Config::LoadPlayerInput(
		GameInput* const input, 
		const Input::PlayerID player)
	{
		return true;
	}

	bool Config::SavePlayerInput(
		GameInput* const input, 
		const Input::PlayerID player)
	{
		return true;
	}

	std::wstring Device::Config::TranslateConfigType(const ConfigType type)
	{
		return configTypeTranslations[(int) type];
	}

	std::wstring Device::Config::MakeConfigTypePath(const ConfigType type)
	{
		return Resource::Interface::MakeResourceFilePath(
			Resource::ResourceType::Settings,
			TranslateConfigType(type)
		);
	}
}
