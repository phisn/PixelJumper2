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

		INVALID
	};

	struct Controls
	{
		const sf::Keyboard::Key controls[(int)Control::INVALID + 1];
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
			return sf::Keyboard::isKeyPressed(controls.controls[(int) control]);
		}
	private:
		Controls controls;
	};
}
