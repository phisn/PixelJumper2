#pragma once

#include <SFML/System.hpp>

namespace Device
{
	enum class GlobalInputSymbol
	{
		Invalid = 0,

		OpenMenu,
		Pause,
		Exit,

		_Length
	};

	enum class LocalInputSymbol
	{
		Invalid = 0,

		// Important
		Trigger,
		Reset,

		Up,
		Down,
		Left,
		Right,

		// View
		SwitchView,		// Whole map or zoom
		ResetView,

		NextTarget,		// Player as view target
		PreviousTarget,

		_Length
	};

	struct GlobalInputSettings
	{
		sf::Keyboard::Key keys[(int) GlobalInputSymbol::_Length - 1];
	};

	const GlobalInputSettings defaultGlobalSettings =
	{
		sf::Keyboard::Key::Tab,
		sf::Keyboard::Key::P,
		sf::Keyboard::Key::Escape
	};

	struct LocalInputSettings
	{
		sf::Keyboard::Key keys[(int) LocalInputSymbol::_Length - 1];
	};

	const LocalInputSettings localInputSettings1 =
	{
		sf::Keyboard::Key::E,
		sf::Keyboard::Key::R,

		sf::Keyboard::Key::W,
		sf::Keyboard::Key::S,
		sf::Keyboard::Key::A,
		sf::Keyboard::Key::D,


		sf::Keyboard::Key::Q,
		sf::Keyboard::Key::F,

		sf::Keyboard::Key::X,
		sf::Keyboard::Key::Y,
	};

	const LocalInputSettings localInputSettings2 =
	{
		sf::Keyboard::Key::Return,
		sf::Keyboard::Key::RShift,

		sf::Keyboard::Key::Up,
		sf::Keyboard::Key::Down,
		sf::Keyboard::Key::Left,
		sf::Keyboard::Key::Right,


		sf::Keyboard::Key::Numpad0,
		sf::Keyboard::Key::Delete,

		sf::Keyboard::Key::RAlt,
		sf::Keyboard::Key::RControl,
	};

	class GlobalInput
	{
	public:
		bool initialize()
		{
			// load from file

			settings = defaultGlobalSettings;

			return true;
		}

		LocalInput loadLocalInput(
			const int position) const
		{
			// load from file

			switch (position)
			{
			case 1:
				return LocalInput(localInputSettings1);
			}
		}

		void saveLocalInput(
			const int position,
			const LocalInput* input) const
		{
		}

		GlobalInputSymbol codeToSymbol(
			sf::Keyboard::Key key)
		{
			static_assert((int) GlobalInputSymbol::_Length > 1, L"Invalid GlobalInputSymbol Elements");

			for (int i = 1; i < (int) GlobalInputSymbol::_Length; ++i)
				if (settings.keys[i - 1] == key)
				{
					return (GlobalInputSymbol) i;
				}

			return GlobalInputSymbol::Invalid;
		}

		void changeSettingsSymbol(
			GlobalInputSymbol symbol,
			sf::Keyboard::Key key)
		{
			settings.keys[(int) symbol - 1] = symbol;

			changed = true;
		}

		void saveSettings()
		{
			if (changed)
			{
				changed = false;

				// ...
			}
		}
	private:
		GlobalInputSettings settings;

		bool changed = false;
	};

	class LocalInput
	{
	public:
		LocalInput(
			const LocalInputSettings localInputSettings)
			:
			localInputSettings(localInputSettings)
		{
		}



	private:
		LocalInputSettings localInputSettings;
	};
}
