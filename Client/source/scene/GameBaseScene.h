#pragma once

#include <Client/source/device/DeviceInterface.h>
#include <Client/source/device/InputDevice.h>

#include <Client/source/game/LocalPlayer.h>
#include <Client/source/game/PlayerBase.h>
#include <Client/source/game/World.h>

#include <Client/source/scene/MainSceneBase.h>
#include <Client/source/framework/FrameworkInterface.h>

#include <Windows.h>

namespace Scene
{
	class GameBase
		:
		public MainBase
	{
	public:
		GameBase(
			GAME::WorldSettings* settings)
			:
			settings(settings)
		{
		}

		_Success_(return == true)
		bool onCreate() override
		{
			world = new GAME::World();

			if (!world->initialize(settings))
			{
				return false;
			}

			return true;
		}
		
		void onRemove() override
		{
			delete world;
		}

		void initialize() override
		{ // init things like scenes or animations
		}

		void onEvent(
			sf::Event event) override
		{
			if (event.type != sf::Event::KeyPressed)
			{
				return;
			}

			switch ( 
				DEVICE::Interface::GetInput()->codeToSymbol(
					event.key.code) 
				) 
			{
			case DEVICE::GlobalInputSymbol::Exit:
				if (!FW::Interface::PopContext())
				{
					MessageBoxW(
						NULL,
						L"Unable to pop context",
						L"Error",
						MB_OK);
				}

				break;
			}
		}

		virtual void onLogic(
			sf::Time time) override
		{
			world->onLogic(time);
		}

		virtual void onDraw() override
		{
			world->onDraw();
		}

	protected:
		void tryDrawForeground() const
		{
			if (isHidden)
			{
				DEVICE::Interface::GetScreen()->onDraw(&background);
			}
		}

		std::vector<
			GAME::PlayerBase*> player;

		GAME::WorldSettings* settings;
		GAME::World* world;

		sf::RectangleShape background;
	private:
		bool isHidden = false;
	};
}
