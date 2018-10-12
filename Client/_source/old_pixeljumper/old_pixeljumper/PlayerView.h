#pragma once

#include "Player.h"

#include <Client/source/device/ScreenDevice.h>

namespace PixelJumper
{
	class PlayerView
	{
	public:
		void adjustView(
			int position,
			int count)
		{
			switch (count)
			{
			case 1:
				view.setViewport(sf::FloatRect(
					0.f, 0.f,
					1.f, 1.f)
				);

				break;
			case 2:
				if (position == 1)
				{
					view.setViewport(sf::FloatRect(
						0.f, 0.f,
						0.5f, 1.f));
				}
				else
				{
					view.setViewport(sf::FloatRect(
						0.5f, 0.f,
						0.5f, 1.f));
				}

				break;
			case 3:
			case 4:
				break;
			}
		}

		void applyView()
		{
			if (player == NULL)
			{
				return;
			}

			view.setCenter(player->getPosition());
			DEVICE::Screen::getWindow()->setView(view);
		}

		void changePlayer(Player* player)
		{
			this->player = player;
		}
	private:
		sf::View view;
		Player* player = NULL;
	};
}
