#pragma once

#include <SFML/Graphics.hpp>

namespace Device
{
	enum class Control
	{
		Up,
		Down,
		Left,
		Right,
		Restart,
		Menu,
		Exit,
	};

	struct Controls
	{
		sf::Keyboard::Key 
			up,
			down,
			left,
			right,
			restart,
			menu,
			exit;
	};

	const Controls CONTROLS_PLAYER1 =
	{
		sf::Keyboard::W,
		sf::Keyboard::S,
		sf::Keyboard::A,
		sf::Keyboard::D,
		sf::Keyboard::R,
		sf::Keyboard::Tab,
		sf::Keyboard::Escape
	};

	const Controls CONTROLS_PLAYER2 =
	{
		sf::Keyboard::Up,
		sf::Keyboard::Down,
		sf::Keyboard::Left,
		sf::Keyboard::Right,
		sf::Keyboard::L,
		(sf::Keyboard::Key) 0, // None
		(sf::Keyboard::Key) 0  // None
	};

	class Input
	{
	public:
		Input(
			const Controls controls)
			:
			controls(controls)
		{
		}

		bool isKeyPressed(Control control)
		{
			switch (control)
			{
			case Control::Up: 
				return controls.up;

			case Control::Down:
				return controls.down;

			case Control::Left:
				return controls.left;

			case Control::Right:
				return controls.right;

			case Control::Restart:
				return controls.restart;

			case Control::Menu:
				return controls.menu;

			case Control::Exit:
				return controls.exit;

			default:
				return false;
			}
		}
	private:
		Controls controls;
	};
}
