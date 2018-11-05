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
			for (Menu::RootBase* const container : containers)
				container->onEvent(event);
		}

		virtual void onLogic(
			const sf::Time time)
		{
			for (Menu::RootBase* const container : containers)
				container->onLogic(time);
		}

		virtual void onDraw()
		{
			for (Menu::RootBase* const container : containers)
				container->onDraw();
		}

	protected:
		std::vector<Menu::RootBase*> containers;
	};
}