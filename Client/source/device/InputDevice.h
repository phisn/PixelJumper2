#pragma once

#include <SFML/Graphics.hpp>

#include <Client/source/resource/ResourceBase.h>

#include <cassert>
#include <deque>

namespace Device
{
	class GameInput;
	namespace Input
	{
		enum class Player
		{
			P1, P2,
			P3, P4,

			_Length
		};

		enum GlobalSymbol
		{
			OpenMenu,
			Pause,
			Exit,

			_Length
		};

		bool Initialize();
		void Uninitialize();

		sf::Keyboard::Key GetGlobalKey(
			const GlobalSymbol symbol);
		bool IsGlobalKeyPressed(
			const GlobalSymbol symbol);
		void SetGlobalKey(
			const GlobalSymbol symbol,
			const sf::Keyboard::Key key);

		int GetKeyUsageCount(
			const sf::Keyboard::Key key);

		bool LoadGlobalKeys();
		bool SaveGlobalKeys();

		void DefaultSettings();

		GameInput* const GetGameInput(const Player player);
	}

	enum class GameCoreInputSymbol
	{
		_Invalid = -1,

		Trigger,
		Reset,

		Up,
		Left,
		Down,
		Right,

		_Length
	};


	enum class GameViewInputSymbol
	{
		_Invalid = -1,

		ChangeZoom,
		ResetZoom,
		WholeMapZoom,

		NextPlayer,
		PreviousPlayer,
		ThisPlayer, // local (real)

		_Length
	};


	struct GeneralInputSettings
		:
		public Resource::ResourceBase
	{
		struct
		{
			sf::Keyboard::Key keys[Input::GlobalSymbol::_Length];

		} GlobalKeys;

		struct
		{
			bool needsCtrlForGlobalKeys; // ?

		} Content;

	private:
		bool make(Resource::ReadPipe* const pipe) override
		{
			return pipe->readValue(&Content)
				&& pipe->readValue(&GlobalKeys);
		}

		bool save(Resource::WritePipe* const pipe) override
		{
			return pipe->writeValue(&Content)
				&& pipe->writeValue(&GlobalKeys);
		}
	};

	class GameInput
		:
		public Resource::ResourceBase
	{
	public:
		GameInput(const Input::Player player)
			:
			player(player)
		{
		}

		bool load();
		bool save();

		bool isKeyPressed(const GameCoreInputSymbol symbol)
		{
			return sf::Keyboard::isKeyPressed(coreKeys[(int)symbol]);
		}

		sf::Keyboard::Key getCoreKey(const GameCoreInputSymbol symbol) const
		{
			return coreKeys[(int) symbol];
		}

		void setCoreKey(
			const GameCoreInputSymbol symbol, 
			const sf::Keyboard::Key key)
		{
			coreKeys[(int) symbol] = key;
		}

		sf::Keyboard::Key getViewKey(const GameViewInputSymbol symbol)
		{
			return viewKeys[(int) symbol];
		}

		void setViewKey(
			const GameCoreInputSymbol symbol,
			const sf::Keyboard::Key key)
		{
			viewKeys[(int) symbol] = key;
		}

	private:
		std::deque<GameCoreInputSymbol> coreSymbols;
		std::deque<GameViewInputSymbol> viewSymbols;

		sf::Keyboard::Key coreKeys[(int) GameCoreInputSymbol::_Length];
		sf::Keyboard::Key viewKeys[(int) GameViewInputSymbol::_Length];

		bool keysPressed[(int)GameCoreInputSymbol::_Length] = { }; // test

		const Input::Player player;

		bool make(Resource::ReadPipe* const pipe) override
		{
			return pipe->readContentForce((char*) coreKeys, sizeof(coreKeys) * sizeof(*coreKeys))
				&& pipe->readContentForce((char*) viewKeys, sizeof(viewKeys) * sizeof(*viewKeys));
		}

		bool save(Resource::WritePipe* const pipe) override
		{
			return pipe->writeContentSafe((char*) coreKeys, sizeof(coreKeys) * sizeof(*coreKeys))
				&& pipe->writeContentSafe((char*) viewKeys, sizeof(viewKeys) * sizeof(*viewKeys));
		}
	};

	class GameInputUser
	{
	protected:
		GameInputUser(const Input::Player player)
			:
			input(Input::GetGameInput(player))
		{
		}

		void onEvent(const sf::Event event)
		{
		}

	protected:
		virtual void onCoreSymbol(const GameCoreInputSymbol) = 0;
		virtual void onViewSymbol(const GameViewInputSymbol) = 0;

	private:
		GameInput* const input;
	};

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
}
