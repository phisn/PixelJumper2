#pragma once

#include <Client/source/menu/RootBase.h>

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
			if (event.type == sf::Event::MouseMoved &&
				!isInside(
					selected,
					event.mouseMove.x,
					event.mouseMove.y)
				)
			{
				for (MENU::RootBase* root : containers)
					if (isInside(
						root,
						event.mouseMove.x,
						event.mouseMove.y))
					{
						selected = root;

						break;
					}
			}

			for (MENU::RootBase* const root : containers)
			{
				root->onEvent(event);
			}
			// selected->onEvent(event);
		}

		virtual void onLogic(
			const sf::Time time)
		{
			for (MENU::RootBase* const root : containers)
			{
				root->onLogic(time);
			}
		}

		virtual void onDraw()
		{
			for (MENU::RootBase* const root : containers)
			{
				root->onDraw();
			}
		}

	protected:
		bool isInside(
			MENU::RootBase* const root,
			const float mouse_x,
			const float mouse_y)
		{
			const float
				x = DEVICE::Interface::getScreen()->getWindow()->getSize().x * root->getView()->getViewport().left,
				y = DEVICE::Interface::getScreen()->getWindow()->getSize().y * root->getView()->getViewport().top,
				width = DEVICE::Interface::getScreen()->getWindow()->getSize().x * root->getView()->getViewport().width,
				height = DEVICE::Interface::getScreen()->getWindow()->getSize().y * root->getView()->getViewport().height;

			return
				mouse_x > x	&& mouse_x < x + width &&
				mouse_y > y	&& mouse_y < y + height;
		}

		Menu::RootBase* selected;

		void addRoot(
			Menu::RootBase* const root)
		{
			containers.push_back(root);

			selected = root;
		}

		const std::vector<Menu::RootBase*>& getContainers() const
		{
			return containers;
		}

	private:
		std::vector<Menu::RootBase*> containers;
	};
}