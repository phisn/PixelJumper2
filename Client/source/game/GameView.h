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
			const sf::Vector2u windowSize = 
				DEVICE::Interface::getScreen()->getWindow()->getSize();

			switch (count)
			{
			case 1:
				size = sf::Vector2f(windowSize);

				view.setViewport(sf::FloatRect(
					0.0f, 0.0f,
					1.0f, 1.0f)
				);

				break;
			case 2:
				size = sf::Vector2f(
					(float) windowSize.x / 2.f,
					windowSize.y);

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
					size = sf::Vector2f(
						(float) windowSize.x / 2.f,
						(float) windowSize.y / 2.f);

					view.setViewport(sf::FloatRect(
						0.0f, 0.0f,
						0.5f, 5.0f));

					break;
				case 2:
					size = sf::Vector2f(
						(float)windowSize.x / 2.f,
						(float)windowSize.y / 2.f);

					view.setViewport(sf::FloatRect(
						0.5f, 0.0f,
						0.5f, 0.5f));

					break;
				case 3:
					size = sf::Vector2f(
						(float)windowSize.x,
						(float)windowSize.y / 2.f);

					view.setViewport(sf::FloatRect(
						0.0f, 0.5f,
						1.0f, 0.5f));

					break;
				}

				break;
			case 4:
				size = sf::Vector2f(
					(float)windowSize.x / 2.f,
					(float)windowSize.y / 2.f);

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
			sf::Vector2f tileSize = sf::Vector2f(
				this->size.x / size.x,
				this->size.y / size.y);

			if (tileSize.x > tileSize.y)
			{
				view.setSize(
					this->size.x / tileSize.y,
					size.y);
			}
			else
			{
				view.setSize(
					size.x,
					this->size.y / tileSize.x);
			}
		}
	private:
		sf::Vector2f size;

		PlayerBase* target;
		sf::View view;
	};
}
