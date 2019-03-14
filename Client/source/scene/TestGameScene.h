#pragma once

#include <Client/source/game/HostWorld.h>
#include <Client/source/game/LocalPlayer.h>

#include <Client/source/resource/ResourceInterface.h>
#include <Client/source/resource/WorldResource.h>

#include <Client/source/scene/MainSceneBase.h>

namespace Scene
{
	class TestGameScene
		:
		public MainSceneBase
	{
	public:
		TestGameScene()
			:
			hostWorld()
		{
		}

		bool onCreate() override
		{
			Game::GameTileFactory::Initialize();

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

			player = Game::LocalPlayer::Create(
				Device::Input::GetGameInput(Device::Input::Player::P1),
				{
					0, 0, 1, 1
				},
				NULL
			);

			if (player == NULL)
			{
				delete player; // TODO: delete?

				return false;
			}

			hostWorld.addPlayer(player);

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
			player->onEvent(event);
		}

		void onLogic(const sf::Time time) override
		{
			hostWorld.onLogic(time);
			player->onLogic(time);
		}

		void onDraw() override
		{
			player->applyView();
			hostWorld.draw();
			Device::Screen::ResetView();
		}

	private:
		Game::HostWorld hostWorld;
		Game::LocalPlayer* player;
	};
}
