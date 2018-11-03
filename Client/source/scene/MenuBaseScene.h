#pragma once

#include <Client/source/menu/MenuContainer.h>

#include <Client/source/scene/MainSceneBase.h>

#include <vector>

namespace Scene
{
	class MenuBase
		:
		public MainBase
	{
	public:
		virtual ~MenuBase() = 0;

		virtual void onEvent(
			const sf::Event event)
		{
			for (Menu::Container* const container : containers)
				if (container->isUseOnEvent())
				{
					container->onEvent(event);
				}
		}

		virtual void onLogic(
			const sf::Time time)
		{
			for (Menu::Container* const container : containers)
				if (container->isUseOnLogic())
				{
					container->onLogic(time);
				}
		}

		virtual void onDraw()
		{
			for (Menu::Container* const container : containers)
				container->onDraw();
		}

	protected:
		std::vector<Menu::Container* const> containers;
	};
}