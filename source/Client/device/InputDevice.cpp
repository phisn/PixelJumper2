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
				&resource,
				Resource::ResourceType::Settings,
				L"input"))
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
			&resource,
			Resource::ResourceType::Settings,
			L"input");
	}

	void Input::ResetInputResource()
	{
		inputResource = DefaultInputResource;
	}

	bool IsGameInputSymbolPressed(const Game::InputSymbol symbol)
	{
		return sf::Keyboard::isKeyPressed((sf::Keyboard::Key) inputResource.gameKeys[(int) symbol]);
	}
}
