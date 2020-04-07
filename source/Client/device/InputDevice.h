#pragma once

#include "GameCore/InputSymbol.h"
#include "ResourceCore/TrivialResource.h"

#include <SFML/Window/Keyboard.hpp>

namespace Device
{
	struct GameInputResourceContent
	{
		sf::Uint8 gameKeys[(int) Game::InputSymbol::_Length];
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
		GameInput(const GameInputResourceContent& content)
			:
			content(content)
		{
		}

		bool isKeyPressed(const Game::InputSymbol symbol) const
		{
			return sf::Keyboard::isKeyPressed((sf::Keyboard::Key) content.gameKeys[(int) symbol]);
		}

	private:
		const GameInputResourceContent& content;
	};

	namespace Input
	{
		struct Player
		{
			enum
			{
				P1,
				P2,
				P3,
				P4
			};
		};

		bool Initialize();
		void Uninitialize();

		InputResourceContent& ChangeInputResource();

		void LoadInputResource();
		bool SaveInputResource();
		void ResetInputResource();

		GameInput GetGameInput(const int player);
	}

	extern const InputResourceContent DefaultInputResource;
}
