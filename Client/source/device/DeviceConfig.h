#pragma once

#include <Client/source/device/ScreenDevice.h>
#include <Client/source/device/InputDevice.h>

namespace Device
{
	enum class ConfigType
	{
		Input,
		Screen,

		_Length
	};

	namespace Config
	{
		bool Initialize();
		void Uninitialize();

		bool LoadType(
			Resource::ResourceBase* const resource,
			const ConfigType type);
		bool SaveType(
			Resource::ResourceBase* const resource,
			const ConfigType type);

		bool LoadPlayerInput(
			Device::GameInput* const input,
			const Input::PlayerID player);
		bool SavePlayerInput(
			Device::GameInput* const input,
			const Input::PlayerID player);

		std::wstring TranslateConfigType(const ConfigType type);
		std::wstring MakeConfigTypePath(const ConfigType type);
	}
}
