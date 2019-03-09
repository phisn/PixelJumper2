#pragma once

#include <Client/source/device/ScreenDevice.h>

#include <Client/source/game/PlayerState.h>

#include <Client/source/resource/PlayerResource.h>

#include <functional>
#include <string>
#include <unordered_set>

#include <SFML/Graphics.hpp>

namespace Game
{
	enum class PlayerType
	{
		Local,
		Net,
		Ghost
	};

	class PlayerBase
		:
		public GameState
	{
		const PlayerType type;
	public:
		PlayerState state;

		PlayerBase(const PlayerType type)
			:
			type(type)
		{
		}

		virtual void draw()
		{
			Device::Screen::Draw(shape);
		}

		bool writeState(
			Resource::WritePipe* const writePipe) override
		{
			return state.writeState(writePipe);
		}

		bool readState(
			Resource::ReadPipe* const readPipe) override
		{
			if (state.readState(readPipe))
			{
				initializeFromState();

				return true;
			}
			else
			{
				return false;
			}
		}
		                                                                                      
		PlayerType getType() const
		{
			return type;
		}

	protected:
		virtual void initializeFromState()
		{
			shape.setPosition(state.readProperties()->position);
			shape.setSize({ 1, 1 });
		}

	private:
		sf::RectangleShape shape;
	};
}
