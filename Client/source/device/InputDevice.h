#pragma once

#include <SFML/Graphics.hpp>

#include <Client/source/resource/ResourceBase.h>

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

	struct GameInputSettings
		:
		public Resource::ResourceBase
	{
		struct
		{
			sf::Keyboard::Key keys[(int) GameCoreInputSymbol::_Length];

		} CoreKeys;

		struct
		{
			sf::Keyboard::Key keys[(int) GameViewInputSymbol::_Length];

		} ViewKeys;

	private:
		bool make(Resource::ReadPipe* const pipe) override
		{
			return pipe->readValue(&CoreKeys)
				&& pipe->readValue(&ViewKeys);
		}

		bool save(Resource::WritePipe* const pipe) override
		{
			return pipe->writeValue(&CoreKeys)
				&& pipe->writeValue(&ViewKeys);
		}
	};

	class GameInput
	{
	public:
		GameInput(const Input::Player player)
		{
		}

		bool load()
		{

		}

		bool save()
		{

		}

		sf::Keyboard::Key getCoreKey(const GameCoreInputSymbol symbol) const
		{
			return coreKeys[(int) symbol];
		}

		bool isCoreKeyPressed(const GameCoreInputSymbol symbol) const
		{
			return sf::Keyboard::isKeyPressed(coreKeys[(int) symbol]);
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

		bool isViewKeyPressed(const GameViewInputSymbol symbol) const
		{
			return sf::Keyboard::isKeyPressed(viewKeys[(int) symbol]);
		}

		void setViewKey(
			const GameCoreInputSymbol symbol,
			const sf::Keyboard::Key key)
		{
			viewKeys[(int) symbol] = key;
		}

	private:
		sf::Keyboard::Key coreKeys[(int) GameCoreInputSymbol::_Length];
		sf::Keyboard::Key viewKeys[(int) GameViewInputSymbol::_Length];
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
