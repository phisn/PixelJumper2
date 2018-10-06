#pragma once

#include <Client/source/game/GameView.h>
#include <Client/source/game/PlayerBase.h>

namespace Game
{
	class LocalPlayer
		:
		public PlayerBase
	{
	public:
		LocalPlayer();
		~LocalPlayer();

		void onLogic(sf::Time time)
		{

		}

		View* getView()
		{
			return &view;
		}
	private:
		View view;
	};
}
