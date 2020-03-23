#pragma once

#include "Client/device/InputDevice.h"

#include "GameCore/LocalPlayer.h"
#include "GameCore/trait/CollidableTrait.h"
#include "FrameworkCore/ScreenDevice.h"

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
			const Game::InputSymbol symbol,
			const LocalRoutine& routine)
			:
			input(input),
			symbol(symbol),
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
			if (currentState = input->isKeyPressed(symbol))
			{
				routine.call();
			}
		}

		template <>
		void handleKey<InputMode::Passive>()
		{
			if (input->isKeyPressed(symbol) != lastState && (lastState = !lastState))
			{
				routine.call();
				currentState = true;
			}
			else
			{
				currentState = false;
			}
		}
		
		bool getCurrentState() const
		{
			return currentState;
		}

	private:
		bool currentState = true;
		bool lastState = false;

		const LocalRoutine& routine;
		const Game::InputSymbol symbol;
		const Device::GameInput* const input;
	};

	class ControllablePlayer
		:
		public LocalPlayer
	{
		// needs to be at top
		// first evaluation
		const Device::GameInput input;

	public:
		ControllablePlayer(
			const int playerID,
			const PlayerInformation information,
			Device::View* const view)
			:
			LocalPlayer(information),
			input(Device::Input::GetGameInput(playerID)),
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
						sf::Vector2f screen_size = sf::Vector2f(Device::Screen::GetWindow()->getSize());

						float size = std::max(newViewWindow.width, newViewWindow.height);

						this->view->setSize(
							{
								this->view->getViewport().width * size,
								this->view->getViewport().height * size
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

		virtual void onInternalUpdate() override
		{
			handleInput();
			PlayerBase::onInternalUpdate();
		}

		KeyController<InputMode::Passive> respawnController{ &input, Game::InputSymbol::Reset, respawn };
		KeyController<InputMode::Passive> interactController{ &input, Game::InputSymbol::Trigger, interact };

		KeyController<InputMode::Active> upController{ &input, Game::InputSymbol::Up, up };;
		KeyController<InputMode::Active> downController{ &input, Game::InputSymbol::Down, down };
		KeyController<InputMode::Active> leftController{ &input, Game::InputSymbol::Left, left };
		KeyController<InputMode::Active> rightController{ &input, Game::InputSymbol::Right, right };

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
