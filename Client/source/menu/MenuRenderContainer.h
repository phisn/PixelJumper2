#pragma once

#include <Client/source/menu/MenuElementBase.h>

namespace Menu
{
	template <typename T = ElementBase>
	class RenderContainer
		:
		public T
	{
		static_assert(std::is_base_of_v<ElementBase, T>,
			"RenderContainer T has to inheret from ElementBase");

	public:
		RenderContainer()
		{
		}

		void onDraw(sf::RenderTarget* const target) const override
		{
			texture.clear(sf::Color::Transparent);
			T::onDraw(&texture);
			texture.display();

			target->draw(sprite);
		}

		// convert child positions to be in rendertexture
		// should not be used by parents
#pragma region RenderContainer_Conversion
		sf::Vector2f convertFullPositionRTV(const sf::Vector2f position) const override
		{
			return position - sf::Vector2f(innerOffset->left, innerOffset->top);
		}

		sf::Vector2f convertFullPositionVTR(const sf::Vector2f position) const override
		{
			return position + sf::Vector2f(innerOffset->left, innerOffset->top);
		}

		sf::Vector2f convertPosition(const sf::Vector2f position) const override
		{
			return position;
		}

		sf::Vector2f convertPositionRTV(const sf::Vector2f position) const override
		{
			return position;
		}

		sf::Vector2f convertPositionVTR(const sf::Vector2f position) const override
		{
			return position;
		}
#pragma endregion

		// RenderContainerReal to Real to convert pseudo 
		// real positions from childs to real positions
		sf::Vector2f convertRcrToReal(const sf::Vector2f position) const
		{
			return ElementBase::convertFullPositionVTR(position);
		}

		sf::Vector2f convertRealToRcr(const sf::Vector2f position) const
		{
			return ElementBase::convertFullPositionRTV(position);
		}

	private:
		void updateOwnGraphics() override
		{
			T::updateOwnGraphics();
			static sf::Vector2f oldSize{ 0, 0 };

			if (*size != oldSize)
			{
				if (!texture.create(size->x, size->y))
				{
					Log::Error(
						L"Failed to create Texture in RenderContainer (width: " +
						std::to_wstring(size->x) +
						L", height: " +
						std::to_wstring(size->y) + L")");
				}

				sprite.setTexture(texture.getTexture());
				oldSize = size;
			}

			sprite.setPosition(ElementBase::convertFullPositionVTR({ 0, 0 }));
		}

		void moveOwnGraphics(const sf::Vector2f offset) override
		{
			T::moveOwnGraphics(offset);
			sprite.move(position);
		}

		sf::Sprite sprite;
		mutable sf::RenderTexture texture;
	};
}