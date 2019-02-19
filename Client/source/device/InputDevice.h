#pragma once

#include <SFML/Graphics.hpp>

namespace Device
{
	namespace Input
	{

	}

	// Same for all players
	enum class GlobalInputSymbol
	{
		Invalid = -1,

		OpenMenu,
		Pause,
		Exit,

		_Length
	};

	// Different for each player
	enum class LocalInputSymbol
	{
		Invalid = -1,

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
		sf::Keyboard::Key keys[(int) GlobalInputSymbol::_Length];
	};

	const GlobalInputSettings defaultGlobalSettings =
	{
		sf::Keyboard::Key::Tab,
		sf::Keyboard::Key::P,
		sf::Keyboard::Key::Escape
	};

	struct LocalInputSettings
	{
		sf::Keyboard::Key keys[(int) LocalInputSymbol::_Length];
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

	class LocalInput;
	class GlobalInput
	{
	public:
		bool initialize();

		LocalInput* loadLocalInput(
			const int position) const;
		void saveLocalInput(
			const int position,
			const LocalInput* input) const;

		void saveSettings() const;

		GlobalInputSymbol codeToSymbol(
			const sf::Keyboard::Key key);

		void changeSettingsSymbol(
			const GlobalInputSymbol symbol,
			const sf::Keyboard::Key key);

		sf::Keyboard::Key getSymbolKey(
			const LocalInputSymbol symbol) const;
	private:
		GlobalInputSettings settings;

		mutable bool changed = false;
	};

	inline void GlobalInput::changeSettingsSymbol(
		const GlobalInputSymbol symbol,
		const sf::Keyboard::Key key)
	{
		settings.keys[(int)symbol] = key;

		changed = true;
	}

	inline sf::Keyboard::Key GlobalInput::getSymbolKey(
		const LocalInputSymbol symbol) const
	{
		return settings.keys[(int)symbol];
	}

	class LocalInput
	{
	public:
		LocalInput(
			const LocalInputSettings settings)
			:
			settings(settings)
		{
		}

		void saveSettings(
			const int position) const;

		void changeSettingsSymbol(
			const LocalInputSymbol symbol,
			const sf::Keyboard::Key key);

		bool isSymbolActive(
			const LocalInputSymbol symbol) const;
		sf::Keyboard::Key getSymbolKey(
			const LocalInputSymbol symbol) const;
	private:
		LocalInputSettings settings;

		mutable bool changed;
	};

	inline bool LocalInput::isSymbolActive(
		const LocalInputSymbol symbol) const
	{
		return sf::Keyboard::isKeyPressed(
			settings.keys[(int) symbol]
		);
	}

	inline void LocalInput::changeSettingsSymbol(
		const LocalInputSymbol symbol,
		const sf::Keyboard::Key key)
	{
		settings.keys[(int) symbol] = key;

		changed = true;
	}

	inline sf::Keyboard::Key LocalInput::getSymbolKey(
		const LocalInputSymbol symbol) const
	{
		return settings.keys[(int)symbol];
	}
}
