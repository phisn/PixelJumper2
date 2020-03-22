#pragma once

#include "GameCore/InputSymbol.h"
#include "ResourceCore/TrivialResource.h"

#include <SFML/Window/Keyboard.hpp>

namespace Device
{
	struct GameInputResourceContent
	{
		sf::Uint8 gameKeys[(int)Game::InputSymbol::_Length];
	};

	struct InputResourceContent
	{
		GameInputResourceContent gameInputs[4];
	};

	struct InputResource
		:
		public Resource::TrivialResource<InputResourceContent>
	{
		bool setup() override
		{
			return true;
		}

		bool validate() override
		{
			return true;
		}
	};

	class GameInput
	{
	public:

	private:
		GameInputResourceContent content;
	};

	namespace Input
	{
		bool Initialize();
		void Uninitialize();

		InputResourceContent& ChangeInputResource();

		void LoadInputResource();
		bool SaveInputResource();
		void ResetInputResource();

		bool IsGameInputSymbolPressed(const Game::InputSymbol symbol);
	}

	extern const InputResourceContent DefaultInputResource;
}
