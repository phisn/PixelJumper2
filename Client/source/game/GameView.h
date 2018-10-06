#pragma once

#include <Client/source/device/DeviceInterface.h>
#include <Client/source/device/ScreenDevice.h>

#include <Client/source/game/PlayerBase.h>

#include <SFML/Graphics/View.hpp>

namespace Game
{
	class View
	{
	public:
		View(PlayerBase* target = NULL)
		{
			changeTarget(target);
		}

		void adjustView(
			int position,
			int count)
		{
			switch (count)
			{
			case 1:
				view.setViewport(sf::FloatRect(
					0.0f, 0.0f,
					1.0f, 1.0f)
				);

				break;
			case 2:
				if (position == 1)
				{
					view.setViewport(sf::FloatRect(
						0.0f, 0.0f,
						0.5f, 1.0f));
				}
				else
				{
					view.setViewport(sf::FloatRect(
						0.5f, 0.0f,
						0.5f, 1.0f));
				}

				break;
			case 3:
				switch (position)
				{
				case 1:
					view.setViewport(sf::FloatRect(
						0.0f, 0.0f,
						0.5f, 0.0f));

					break;
				case 2:
					view.setViewport(sf::FloatRect(
						0.5f, 0.0f,
						0.5f, 0.5f));

					break;
				case 3:
					view.setViewport(sf::FloatRect(
						0.0f, 0.5f,
						1.0f, 0.5f));

					break;
				}

				break;
			case 4:
				switch (position)
				{
				case 1:
					view.setViewport(sf::FloatRect(
						0.0f, 0.0f,
						0.5f, 0.0f));

					break;
				case 2:
					view.setViewport(sf::FloatRect(
						0.5f, 0.0f,
						0.5f, 0.5f));

					break;
				case 3:
					view.setViewport(sf::FloatRect(
						0.0f, 0.5f,
						0.5f, 0.5f));

					break;
				case 4:
					view.setViewport(sf::FloatRect(
						0.5f, 0.5f,
						0.5f, 0.5f));

					break;
				}

				break;
			}
		}

		void apply()
		{
			if (target)
			{
				view.setCenter(target->getPosition());
			}

			DEVICE::Interface::getScreen()->applyView(&view);
		}

		void changeTarget(
			PlayerBase* target)
		{
			this->target = target;

			if (target == NULL)
			{
				view.setCenter(view.getCenter() / 2.f);
			}
		}

		void setSize(
			sf::Vector2f size)
		{
			view.setSize(size);
		}
	private:
		PlayerBase* target;
		sf::View view;
	};
}
