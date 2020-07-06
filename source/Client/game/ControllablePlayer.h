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
				[this]()
				{
					properties.viewWindow.update(true);
				});
			properties.viewWindow.addListener(
				[this]()
				{
					if (properties.viewFollow)
					{
						// sf::Vector2f screen_size = sf::Vector2f(Device::Screen::GetWindow()->getSize());

						// float size = std::max(newViewWindow.width, newViewWindow.height);

						this->view->setSize(
							{
								properties.viewWindow->width,
								properties.viewWindow->height
							});
					}
					else
					{
						this->view->setRect(properties.viewWindow);
					}
				});
			properties.viewRotation.addListener(
				[this]()
				{
					this->view->setRotation(properties.viewRotation);
				});
			properties.position.addListener(
				[this]()
				{
					if (properties.viewFollow)
					{
						this->view->setCenter(properties.position);
					}
				});
		}

		virtual void onInternalUpdate() override
		{
			handleInput();
			LocalPlayer::onInternalUpdate();
		}

		KeyController<InputMode::Passive> respawnController{ &input, Game::InputSymbol::Reset, respawn };
		KeyController<InputMode::Passive> interactController{ &input, Game::InputSymbol::Trigger, interact };

		KeyController<InputMode::Active> upController{ &input, Game::InputSymbol::Up, up };;
		KeyController<InputMode::Active> downController{ &input, Game::InputSymbol::Down, down };
		KeyController<InputMode::Active> leftController{ &input, Game::InputSymbol::Left, left };
		KeyController<InputMode::Active> rightController{ &input, Game::InputSymbol::Right, right };

		void enableView(sf::RenderTarget* const target) const
		{
			view->enable(target);
		}

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
