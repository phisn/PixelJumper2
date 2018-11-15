#pragma once

#include <Client/source/menu/element/ScrollBarBase.h>
#include <Client/source/menu/RootBase.h>

#include <vector>

class ScrollBar
	:
	public MENU::ScrollBarBase
{
public:
	ScrollBar()
		:
		MENU::ScrollBarBase(
			{
				sf::Color::Color(100, 100, 100),
				sf::Color::Color(50, 50, 50),

				sf::Color::Color(150, 150, 150),
				sf::Color::Color(50, 50, 50),

				sf::Color::Color(200, 200, 200),
				sf::Color::Color(50, 50, 50),
			},
			MENU::Direction::Vertical)
	{
	}

	void onLogic(const sf::Time time) override { }
	void onScrollBarMoved() override { updatePosition(); }
};

namespace Editor
{
	class TileMenu
		:
		public MENU::RootBase
	{
	public:
		TileMenu()
		{
		}

		void onEvent(
			const sf::Event event) override
		{
			s.onEvent(event);
		}

		void onLogic(
			const sf::Time time) override { }

		void onDraw() override
		{
			MENU::RootBase::onDraw();

			s.onDraw();
		}

		void addTile(
			TileTemplate* const tile)
		{

		}

		void resetLayout() override
		{
			MENU::ScrollBarBase::Properties p;

			p.padding = 4.f;
			p.position = { 0.0f, 0.0f };
			p.size = sf::Vector2f(
				50.f, 
				DEVICE::Interface::getScreen()->getWindow()->getSize().y 
			);
			p.view = getView();

			s.initialize(&p);
			s.setConsumption(0.5);
		}

		/*
		void resetLayout() override
		{
			// all values as pixel
			// setup values
			const float scrollBarSize = 30.f;
			const float scrollBarWidth = 2.f;
			const float containerDistance = 0.f;

			// setup properties
			static Container::Properties properties;

			properties.position = { 0.f, 0.f };
			properties.size =
			{
				DEVICE::Interface::getScreen()->getWindow()->getSize().x * getView()->getViewport().width,
				DEVICE::Interface::getScreen()->getWindow()->getSize().y * getView()->getViewport().height
			};

			properties.scrollBar.padding = 2.f;
			properties.scrollBar.position = { 0.f, 0.f };
			properties.scrollBar.size =
			{
				scrollBarSize,
				properties.size.y
			};

			properties.container.position = 
			{ 
				scrollBarSize + containerDistance,
				0.f 
			};
			properties.container.size =
			{
				properties.size.x - scrollBarSize,
				properties.size.y
			};

			TileMenuButton::Properties buttonProp;

			// setup container elements
			for (TileMenuButton& element : elements)
			{
				buttonProp.position = { 0, 0 };
				buttonProp.size =
				{
					properties.size.x,
					properties.size.y / 10.f
				};

				element.initialize(
					&buttonProp);
				container.addElement(&element);
			}

			container.initialize(&properties);
			container.loadPosition();
		}*/

	private:
		ScrollBar s;
		/*
		void createTiles()
		{
			tiles.push_back( new WallTemplate() );
		}

		void createElements()
		{
			TileMenuButton::Style style;

			style.clickColor = sf::Color(
				tiles[elements.size()]->color.toInteger() - 0x05050500
			);
			style.fillColor = 
				tiles[elements.size()]->color;
			style.label = 
				tiles[elements.size()]->name;

			elements.emplace_back(&container, style);

			// ...
			// add from templates
		}


		std::vector<
			TileTemplate*> tiles;
		std::vector<
			TileMenuButton> elements;
			*/

	};
}
