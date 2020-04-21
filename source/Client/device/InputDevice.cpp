#include "InputDevice.h"
#include "Logger/Logger.h"
#include "ResourceCore/ResourceInterface.h"

namespace
{
	Device::InputResourceContent inputResource;
}

const Device::InputResourceContent Device::DefaultInputResource = 
{
	sf::Keyboard::E,
	sf::Keyboard::R,
	sf::Keyboard::W,
	sf::Keyboard::A,
	sf::Keyboard::S,
	sf::Keyboard::D
};

namespace Device
{
	bool Input::Initialize()
	{
		if (InputResource resource; Resource::Interface::LoadResource(
				L"input",
				&resource,
				Resource::SettingsResourceDefinition))
		{
			inputResource = resource.content;
		}
		else
		{
			Log::Warning(L"screen resource not found. creating new one");

			ResetInputResource();
			if (!SaveInputResource())
				Log::Error(L"failed to save new screen resource");
		}

		LoadInputResource();

		return true;
	}

	void Input::Uninitialize()
	{
	}

	InputResourceContent& Input::ChangeInputResource()
	{
		return inputResource;
	}

	void Input::LoadInputResource()
	{
	}

	bool Input::SaveInputResource()
	{
		InputResource resource;
		resource.adopt(inputResource);
		return Resource::Interface::SaveResource(
			L"input",
			&resource,
			Resource::SettingsResourceDefinition);
	}

	void Input::ResetInputResource()
	{
		inputResource = DefaultInputResource;
	}

	GameInput Input::GetGameInput(const int player)
	{

		GameInput input{ inputResource.gameInputs[player] };
		return input;
	}
}
