#pragma once

#include <Client/source/device/InputDevice.h>
#include <Client/source/device/ScreenDevice.h>

#include <Client/source/game/LocalPlayer.h>
#include <Client/source/game/tiletrait/CollidableTile.h>

namespace Game
{
	enum class InputMode
	{
		Active,  // (pull  based) repeated with time
		Passive  // (event based) not repeated without time
	};

	template <InputMode mode>
	class KeyController
	{
		typedef InputRoutine<> LocalRoutine;

	public:
		KeyController(
			const Device::GameInput* const input,
			const Device::GameCoreInputSymbol key,
			const LocalRoutine& routine)
			:
			input(input),
			key(key),
			routine(routine)
		{
		}

		template <InputMode = mode>
		void handleKey()
		{
		}

		template <>
		void handleKey<InputMode::Active>()
		{
			currentState = input->isKeyPressed(key);

			if (currentState)
			{
				routine.call();
			}
		}

		template <>
		void handleKey<InputMode::Passive>()
		{
			if (input->isKeyPressed(key) != currentState && (currentState = !currentState))
			{
				routine.call();
			}
		}
		
		bool getCurrentState() const
		{
			return currentState;
		}

	private:
		bool currentState = false;

		const LocalRoutine& routine;
		const Device::GameCoreInputSymbol key;
		const Device::GameInput* const input;
	};

	class ControllablePlayer
		:
		public LocalPlayer
	{
		// needs to be at top
		// first evaluation
		Device::GameInput* const input;

	public:
		ControllablePlayer(
			const Device::Input::PlayerId playerId,
			const PlayerInformation information,
			Device::View* const view)
			:
			LocalPlayer(information),
			input(Device::Input::GetGameInput(playerId)),
			view(view)
		{
			properties.viewFollow.addListener(
				[this](const bool oldViewFollow,
					   const bool newViewFollow)
				{
					properties.viewWindow.update(true);
				});
			properties.viewWindow.addListener(
				[this](const sf::FloatRect oldViewWindow,
					   const sf::FloatRect newViewWindow)
				{
					if (properties.viewFollow)
					{
						this->view->setSize(
							{
								properties.viewWindow->width,
								properties.viewWindow->height
							});
					}
					else
					{
						this->view->setRect(newViewWindow);
					}
				});
			properties.viewRotation.addListener(
				[this](const float oldViewRotation,
					   const float newViewRotation)
				{
					this->view->setRotation(newViewRotation);
				});
			properties.position.addListener(
				[this](const sf::Vector2f oldPosition,
					   const sf::Vector2f newPosition)
				{
					if (properties.viewFollow)
					{
						this->view->setCenter(newPosition);
					}
				});
		}

		void onInternalUpdate() override
		{
			handleInput();
			PlayerBase::onInternalUpdate();
		}

		KeyController<InputMode::Passive> respawnController{ input, Device::GameCoreInputSymbol::Reset, respawn };
		KeyController<InputMode::Passive> interactController{ input, Device::GameCoreInputSymbol::Trigger, interact };

		KeyController<InputMode::Active> upController{ input, Device::GameCoreInputSymbol::Up, up };;
		KeyController<InputMode::Active> downController{ input, Device::GameCoreInputSymbol::Down, down };
		KeyController<InputMode::Active> leftController{ input, Device::GameCoreInputSymbol::Left, left };
		KeyController<InputMode::Active> rightController{ input, Device::GameCoreInputSymbol::Right, right };

	private:
		void handleInput()
		{
			respawnController.handleKey();
			interactController.handleKey();

			upController.handleKey();
			downController.handleKey();
			leftController.handleKey();
			rightController.handleKey();
		}

		Device::View* const view;
	};
}
