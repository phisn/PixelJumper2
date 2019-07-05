#pragma once

#include <Client/source/device/ScreenDevice.h>

#include <Client/source/game/CollisionEngine.h>
#include <Client/source/game/PlayerState.h>

#include <Client/source/resource/PlayerResource.h>

#include <functional>
#include <string>
#include <unordered_set>

#include <Client/source/logger/Logger.h>
#include <SFML/Graphics.hpp>

namespace Game
{
	// return in percent lose of movement
	static float CalculateFrictionLose(const float friction, const float weight)
	{
		const float positiveTileDragValue = fabs(friction);
		const float positivePlayerWeight = fabs(weight);

		return positivePlayerWeight / (positiveTileDragValue + positivePlayerWeight);
	}

	static float FrictionLoseOverTime(const float lose, const float timeValue)
	{
		return std::powf(lose, timeValue);
	}

	static sf::Vector2f ApplyFriction(
		const PlayerState* const player,
		const float additionalFriction,
		const float timeValue)
	{
		const PlayerStateProperties* const properties = player->readProperties();
		const float friction = additionalFriction + properties->friction;

		sf::Vector2f movement = properties->movement;

		movement *= FrictionLoseOverTime(
			CalculateFrictionLose(
				friction,
				properties->weight),
			timeValue / 70.f);

		const float static_lose = friction * timeValue * 1.f;

		if (!player->readProperties()->hasForceLeft && !player->readProperties()->hasForceRight)
		{
			if (fabsf(movement.x) < static_lose)
			{
				movement.x = 0;
			}
			else
			{
				movement.x += movement.x < 0
					? static_lose
					: -static_lose;
			}

			if (fabsf(movement.y) < static_lose)
			{
				movement.y = 0;
			}
			else
			{
				movement.y += movement.y < 0
					? static_lose
					: -static_lose;
			}
		}

		return movement;
	}

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
				[this](const sf::Vector2f newPosition,
					   const sf::Vector2f oldPosition)
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
