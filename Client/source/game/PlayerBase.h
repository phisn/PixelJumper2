#pragma once

#include <Client/source/device/ScreenDevice.h>

#include <Client/source/game/CollisionEngine.h>
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

	class CollidableTile;
	class CollisionContainer
	{
	public:
		bool has(
			const CollisionEngine::CollisionInfo::Type type) const
		{
			return !!infos[type];
		}

		CollidableTile* operator[](
			const CollisionEngine::CollisionInfo::Type type) const
		{
			return infos[type];
		}

		void clear()
		{
			infos[0] = { };
			infos[1] = { };
			infos[2] = { };
			infos[3] = { };
		}

		void setCollision(
			const CollisionEngine::CollisionInfo::Type type,
			CollidableTile* const tile)
		{
			infos[type] = tile;
		}

	private:
		CollidableTile* infos[4];
	};

	class PlayerBase
		:
		public GameState
	{
		friend class WorldBase;
		const PlayerType type;
	public:
		PlayerState state;

		PlayerBase(const PlayerType type)
			:
			type(type)
		{
			shape.setSize({ 1, 1 });
			shape.setPosition({ 1, 1 });
			shape.setFillColor(sf::Color::Red);

			state.position.addListener(
				[this](const sf::Vector2f old)
				{
					this->isRepresentationInvalid = true;
				});
		}

		virtual void onLogic(const sf::Time) = 0;

		virtual void draw()
		{
			if (isRepresentationInvalid)
			{
				updateRepresentation();
			}

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

		CollisionContainer collisionContainer;

	private:
		void updateRepresentation()
		{
			shape.setPosition(state.readProperties()->position);
		}

		bool isRepresentationInvalid = true;
		sf::RectangleShape shape;
	};
}
