#pragma once

#include <Client/source/menu/MenuRootBase.h>

#include <Client/source/scene/MainSceneBase.h>

#include <vector>

namespace Scene
{
	class MenuBaseScene
		:
		public MainSceneBase
	{
	public:
		virtual bool onCreate() override
		{
			for (Menu::MenuRootBase* const root : roots)
				if (!root->build())
				{
					return false;
				}

			return true;
		}

		virtual void onRemove() override
		{
			
		}

		virtual void initialize() override
		{
			for (Menu::MenuRootBase* const root : roots)
				root->initialize();
		}

		virtual void onShow() override
		{
		}

		virtual void onHide() override
		{
		}

		virtual void onEvent(const sf::Event event) override
		{
			switch (event.type)
			{
			case sf::Event::MouseButtonPressed:
				if (Menu::MenuRootBase * const root = findTargetRoot(
						event.mouseButton.x,
						event.mouseButton.y,
						strongSelected);
					root != strongSelected)
				{
					strongSelected->unselectStrong();
					strongSelected = root;
					strongSelected->selectStrong();
				}

				// fall though
			case sf::Event::TextEntered:
				if (strongSelected)
				{
					strongSelected->onEvent(event);
				}

				break;
			case sf::Event::MouseMoved:
				if (Menu::MenuRootBase* const root = findTargetRoot(
						event.mouseMove.x,
						event.mouseMove.y,
						weakSelected); 
					root != weakSelected)
				{
					weakSelected->onEvent(event);

					weakSelected->unselectWeak();
					weakSelected = root;
					weakSelected->selectWeak();
				}

				// fall though
			case sf::Event::EventType::MouseWheelMoved:
			case sf::Event::EventType::MouseWheelScrolled:
				weakSelected->onEvent(event);
				
				break;
			default:
				for (Menu::MenuRootBase* const root : roots)
					root->onEvent(event);

				break;
			}
		}

		virtual void onLogic(const sf::Time time) override
		{
			for (Menu::MenuRootBase* const root : roots)
				root->onLogic(time);
		}

		virtual void onDraw(sf::RenderTarget* const target) override
		{
			for (Menu::MenuRootBase* const root : roots)
				root->draw(target);
		}

	protected:
		typedef std::vector<Menu::MenuRootBase*> RootContainer;

		template <class Root, class... Args>
		Root* addRoot(Args... arguments)
		{
			static_assert(
				std::is_base_of_v<Menu::MenuRootBase, Root>, 
				"Invalid root added to menu");

			Root* root = new Root(arguments...);
			roots.push_back(
				weakSelected = strongSelected = root
			);
			return root;
		}

		const RootContainer& getRoots() const
		{
			return roots;
		}

		Menu::MenuRootBase* getWeakSelected() const
		{
			return weakSelected;
		}

		Menu::MenuRootBase* getStrongSelected() const
		{
			return strongSelected;
		}

	private:
		void onKeyPressed(const sf::Event event)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::Key::Tab:
			case sf::Keyboard::Key::Enter:
			case sf::Keyboard::Key::Up:
			case sf::Keyboard::Key::Left:
			case sf::Keyboard::Key::Down:
			case sf::Keyboard::Key::Right:
				break;
			}
		}

		Menu::MenuRootBase* findTargetRoot(
			const float x, 
			const float y,
			Menu::MenuRootBase* expected)
		{
			assert(expected != NULL); // in menubase cant be null

			const sf::Vector2f position{ x, y };

			if (isTargetChild(position, expected))
			{
				return expected;
			}

			for (Menu::MenuRootBase* const root : roots)
			{
				if (isTargetChild(position, root))
					return root;
			}

			return expected;
		}

		bool isTargetChild(
			const sf::Vector2f position,
			const Menu::MenuRootBase* const root)
		{
			return Menu::MenuRootBase::ConvertPortToReal(root->viewPort).contains(position);
		}

		Menu::MenuRootBase* weakSelected;
		Menu::MenuRootBase* strongSelected;

		RootContainer roots;
	};

	/*class SubMenuBase
		:
		public SubBase
	{
	public:
	};*/

	/*
	class _MenuBase
		:
		public MainSceneBase
	{
	public:
		virtual ~_MenuBase() { };

		virtual void onEvent(
			const sf::Event event)
		{
			/*
			
				Create strong select and weak select
				Strong: MouseButtonPressed -> for important events
				Weak:   MouseMoved -> for visual events
			
			if (event.type == sf::Event::MouseButtonPressed &&
				!isInside(
					selected,

					(float) event.mouseButton.x,
					(float) event.mouseButton.y)
				)
			{
				for (MENU::RootBase* root : containers)
					if (isInside(
							root,
							(float) event.mouseButton.x,
							(float) event.mouseButton.y)
						)
					{
						selected = root;

						break;
					}
			}

			switch (event.type)
			{
			// case ...:
			case sf::Event::MouseMoved:
			case sf::Event::MouseWheelScrolled:
				for (MENU::RootBase* const root : containers)
				{
					root->onEvent(event);
				}

				break;
			default:
				selected->onEvent(event);

				break;
			}
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
				x = Device::Screen::GetWindow()->getSize().x * root->getView()->getViewport().left,
				y = Device::Screen::GetWindow()->getSize().y * root->getView()->getViewport().top,
				width =		Device::Screen::GetWindow()->getSize().x * root->getView()->getViewport().width,
				height =	Device::Screen::GetWindow()->getSize().y * root->getView()->getViewport().height;

			return
				mouse_x > x	&& mouse_x < x + width &&
				mouse_y > y	&& mouse_y < y + height;
		}

		Menu::RootBase* selected;

		template <class Root, class... Args>
		Root* addRoot(Args... arguments)
		{
			Root* root = new Root(arguments...);
			containers.push_back(selected = root);
			return root;
		}

		const std::vector<Menu::RootBase*>& getContainers() const
		{
			return containers;
		}

	private:
		std::vector<Menu::RootBase*> containers;
	};*/
}