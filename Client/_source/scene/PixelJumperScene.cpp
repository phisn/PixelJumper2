#include "PixelJumperScene.h"

namespace Scene
{
	bool PixelJumper::onCreate()
	{
		shape.setFillColor(sf::Color::White);
		shape.setSize(sf::Vector2f(200.f, 200.f));

		return context->initialize();
	}

	bool PixelJumper::onRemove()
	{
		return true;
	}

	void PixelJumper::onShow()
	{
	}

	void PixelJumper::onHide()
	{
	}

	void PixelJumper::onEvent(
		sf::Event event)
	{
		switch (event.type)
		{
		case sf::Event::Resized:
			break;
		}
	}

	void PixelJumper::onLogic(
		sf::Time time)
	{
		for (PJ::LocalPlayer* player : context->localPlayers)
		{
			player->onUpdate(time);
		}

		for (PJ::RemotePlayer* player : context->remotePlayers)
		{
			player->onUpdate(time);
		}

		context->map->onUpdate(time);
	}

	void PixelJumper::onDraw()
	{
		for (PJ::LocalPlayer* localPlayer : context->localPlayers)
		{
			localPlayer->getView().applyView();

			DEVICE::Screen::getWindow()->draw(shape);
			context->map->draw();

			for (PJ::Player* player : context->remotePlayers)
			{
				player->draw();
			}

			for (PJ::Player* player : context->localPlayers)
			{
				player->draw();
			}
		}
	}
}
