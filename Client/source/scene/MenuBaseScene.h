#pragma once

#include <Client/source/menu/MenuRoot.h>

#include <Client/source/scene/MainSceneBase.h>

#include <vector>

namespace Scene
{
	class MenuBase
		:
		public MainBase
	{
	public:
		virtual ~MenuBase() { };

		virtual void onEvent(
			const sf::Event event)
		{
			for (Menu::Root* const container : containers)
				if (container->isUseOnEvent())
				{
					container->onEvent(event);
				}
		}

		virtual void onLogic(
			const sf::Time time)
		{
			for (Menu::Root* const container : containers)
				if (container->isUseOnLogic())
				{
					container->onLogic(time);
				}
		}

		virtual void onDraw()
		{
			for (Menu::Root* const container : containers)
				container->onDraw();
		}

	protected:
		std::vector<Menu::Root*> containers;
	};
}