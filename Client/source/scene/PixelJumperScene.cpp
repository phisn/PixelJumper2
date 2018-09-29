#include "PixelJumperScene.h"

namespace Scene
{
	bool PixelJumper::onCreate()
	{
		return true;
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
		for (PJ::Player* viewedPlayer : context->viewedPlayers)
		{
			viewedPlayer->applyView();

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
