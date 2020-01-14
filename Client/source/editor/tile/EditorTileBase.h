#pragma once

#include <Client/source/device/ScreenDevice.h>
#include <Client/source/resource/TileResource.h>
#include <Client/source/shared/tiles/TileCommon.h>

#include <SFML/Graphics.hpp>

namespace Editor
{
	class TileBase
	{
	public:
		TileBase(
			const Shared::TileID id,
			const sf::Color commonColor)
			:
			id(id),
			color(commonColor)
		{
		}

		virtual void setPosition(const sf::Vector2f position)
		{
			this->position = position;
		}

		virtual void setSize(const sf::Vector2f size)
		{
			this->size = size;
		}

		sf::Vector2f getPosition() const
		{
			return position;
		}

		sf::Vector2f getSize() const
		{
			return size;
		}

		Shared::TileID getTileId() const
		{
			return id;
		}

		sf::Color getColor() const
		{
			return color;
		}

		virtual bool equals(TileBase* const tile) const
		{
			return id == tile->id;
		}

		virtual bool adopt(const Resource::TileInstanceWrapper* const instanceWrapper) = 0;
		virtual void assignInstance(const Resource::TileInstanceWrapper* const instanceWrapper) const = 0;

		virtual void draw(sf::RenderTarget* const target) const = 0;

	protected:
		sf::Color color;

	private:
		sf::Vector2f size{ 1, 1 }, position{ 0, 0 };

		const Shared::TileID id;
	};
}