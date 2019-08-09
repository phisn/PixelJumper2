#pragma once

#include <Client/source/game/HostWorld.h>
#include <Client/source/game/LocalPlayer.h>

#include <Client/source/resource/ResourceInterface.h>
#include <Client/source/resource/WorldResource.h>

#include <Client/source/scene/MainSceneBase.h>

#include <Client/source/framework/FrameworkInterface.h>


namespace Scene
{
	class TestGameScene
		:
		public MainSceneBase
	{
	public:
		TestGameScene(Resource::World* const world = NULL)
			:
			hostWorld(),
			worldResource(world)
		{
		}

		bool onCreate() override
		{
			if (worldResource == NULL)
			{
				Resource::World world;

				if (!Resource::Interface::LoadResource(
					&world,
					Resource::ResourceType::World,
					L"TestWorld"))
				{
					return false;
				}

				if (!hostWorld.initialize(&world))
				{
					return false;
				}
			}
			else
			{
				if (!hostWorld.initialize(worldResource))
				{
					return false;
				}
			}

			player = Game::LocalPlayer::Create(
				Device::Input::GetGameInput(Device::Input::Player::P1),
				{
					0, 0, 1, 1
				},
				NULL
			);

			if (player == NULL)
			{
				return false;
			}

			hostWorld.addPlayer(player);
			player->setCurrentWorld(&hostWorld);

			return true;
		}

		void onRemove() override
		{
			delete player;
		}

		void initialize() override
		{
		}

		void onShow() override
		{
		}

		void onHide() override
		{
		}

		void onScenePopped(const int size) override
		{
		}

		void onEvent(const sf::Event event) override
		{
			if (event.type == sf::Event::Closed)
			{
				Framework::Context::Pop();

				return;
			}

			player->onEvent(event);

			if (event.type == sf::Event::KeyPressed &&
				event.key.code == sf::Keyboard::Key::T &&
				event.key.control)
			{
				Framework::Context::Pop();
			}
		}

		void onLogic(const sf::Time time) override
		{
			player->onLogic(time);
			hostWorld.onLogic(time);
		}

		void onDraw(sf::RenderTarget* const target) override
		{
			player->applyView();
			hostWorld.draw(target);
			Device::Screen::ResetView();
		}

	private:
		Game::HostWorld hostWorld;
		Game::LocalPlayer* player;

		Resource::World* const worldResource;
	};
}
