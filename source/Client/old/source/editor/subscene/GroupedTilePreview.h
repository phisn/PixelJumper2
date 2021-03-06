#pragma once

#include <Client/source/Common.h>
#include <Client/source/framework/FrameworkInterface.h>

#include <Client/source/device/RandomDevice.h>
#include <Client/source/device/ScreenDevice.h>

#include <Client/source/editor/manipulator/EditorWorld.h>
#include <Client/source/editor/manipulator/Manipulator.h>

#include <Client/source/resource/ResourceInterface.h>
#include <client/source/resource/WorldResource.h>

#include <Client/source/scene/SubSceneBase.h>

#include <vector>

#include <SFML/Graphics.hpp>


namespace Editor
{
	class GroupedTilePreview
		:
		public Scene::SubBase
	{
	public:
		bool onCreate() override
		{
			if (Editor::Manipulator::GetWorld()->getTiles().empty())
			{
				Log::Error(L"Unable to load GroupedTilePreview of empty world");

				return false;
			}

			sf::Image groupedTiles;
			Resource::World* const groupedWorld = Editor::Manipulator::GetWorld()->convert(0xff, 0xff);
			groupedTiles.create(
				groupedWorld->content.width,
				groupedWorld->content.height);
			
			for (const Resource::Tile& groupedTile : groupedWorld->tiles)
			{
				sf::Uint32 rawRandomColor = Device::Random::MakeRandom<int>();
				rawRandomColor += 0x222222'00;

				if (rawRandomColor & 0xff0000'00 < 0x220000'00)
				{
					rawRandomColor += 0x2200000'00;
				}

				if (rawRandomColor & 0x00ff00'00 < 0x002200'00)
				{
					rawRandomColor += 0x002200'00;
				}

				if (rawRandomColor & 0x0000ff'00 < 0x000022'00)
				{
					rawRandomColor += 0x000022'00;
				}

				rawRandomColor |= 0x000000'ff;
				const sf::Color randomColor = (sf::Color) rawRandomColor;

				for (int x = 0; x < groupedTile.content.width; ++x)
					for (int y = 0; y < groupedTile.content.height; ++y)
					{
						groupedTiles.setPixel(
							x + groupedTile.content.x,
							y + groupedTile.content.y,
							randomColor
						);
					}

				if (!groupedTilesTexture.loadFromImage(groupedTiles))
				{
					return false;
				}

				groupedTilesSprite.setTexture(groupedTilesTexture);
				groupedTilesSprite.setScale(40.f, 40.f);
			}

			delete groupedWorld;
			return true;
		}

		void onRemove() override 
		{ 
		}

		void onEvent(const sf::Event event) override
		{
			if (event.type == sf::Event::Closed)
			{
				Framework::Context::PopScene();

				return;
			}

			if (event.type == sf::Event::KeyPressed)
			{
				switch (event.key.code)
				{
				case sf::Keyboard::Escape:
				case sf::Keyboard::Return:
					Framework::Context::PopScene();

					break;
				case sf::Keyboard::W:
				case sf::Keyboard::Up:
					groupedTilesSprite.move(0.f, -20.0f);

					break;
				case sf::Keyboard::A:
				case sf::Keyboard::Left:
					groupedTilesSprite.move(-20.f, 0.f);

					break;
				case sf::Keyboard::S:
				case sf::Keyboard::Down:
					groupedTilesSprite.move(0.f, 20.f);

					break;
				case sf::Keyboard::D:
				case sf::Keyboard::Right:
					groupedTilesSprite.move(20.f, 0.f);

					break;
				}
			}
		}
		
		void onLogic(const sf::Time time) override
		{
		}
		
		virtual void onDraw(sf::RenderTarget* const target) override
		{
			target->draw(groupedTilesSprite);
		}

	private:
		sf::Texture groupedTilesTexture;
		sf::Sprite groupedTilesSprite;
	};
}
