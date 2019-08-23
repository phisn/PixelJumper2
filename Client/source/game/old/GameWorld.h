#pragma once

#include <Client/source/device/ScreenDevice.h>
#include <Client/source/logger/Logger.h>

#include <Client/source/game/WorldState.h>
#include <Client/source/game/tiletrait/GameTileBase.h>

#include <Client/source/resource/WorldResource.h>

namespace Game
{
	class GameWorld
	{
		GameWorld() = default;
	public:
		WorldState worldState; // TODO: Open?

		static GameWorld* Create(
			Resource::World* const worldResource);

		Resource::World* convert() const
		{
			return NULL;
		}

		void onDraw() const;
		void onLogic(const sf::Time time) const;

		sf::Vector2f getSize() const;
		const std::wstring& getAuthorName() const;
		const std::wstring& getMapName() const;
	private:
		struct
		{
			sf::Vector2f size;

			std::wstring authorName;
			std::wstring mapName;
		} Properties;

		struct
		{
			sf::Texture texture;
			sf::Sprite sprite;
		} Visual;
	};

	inline sf::Vector2f GameWorld::getSize() const
	{
		return Properties.size;
	}

	inline const std::wstring& GameWorld::getAuthorName() const
	{
		return Properties.authorName;
	}

	inline const std::wstring& GameWorld::getMapName() const
	{
		return Properties.mapName;
	}
}
