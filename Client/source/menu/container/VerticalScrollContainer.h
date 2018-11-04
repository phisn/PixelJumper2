#pragma once

#include <Client/source/menu/container/SimpleContainer.h>
#include <Client/source/menu/element/SimpleScrollBar.h>

namespace Menu
{
	class VerticalScrollContainer
		:
		public ContainerBase
	{
	public:
		struct Style
		{
			sf::Vector2f
				size,
				position;
		};

		VerticalScrollContainer(
			ElementBase* const parent,
			const Style style,
			SimpleContainer::Style containerStyle,
			SimpleScrollBar::Style scrollBarStyle,
			const sf::View* const view)
			:
			container(
				this,
				containerStyle,
				SimpleContainer::Direction::Vertical,
				this),
			scrollBar(
				this,
				scrollBarStyle,
				SimpleScrollBar::Direction::Vertical,
				view,
				this),
			ContainerBase(
				NULL),
			style(style)
		{
		}

		void onEvent(
			const sf::Event event) override
		{

		}

		void onLogic(
			const sf::Time time) override
		{

		}

		void onDraw() override
		{

		}

		sf::Vector2f getSize() const override
		{
			return style.size;
		}
		
		sf::Vector2f getPosition() const override
		{
			return position;
		}

		void loadPosition() override
		{
			this->position = convertPosition(style.position);

			scrollBar.loadPosition();
			container.loadPosition();
		}

		void addElement(
			ElementBase* const element) override
		{
			container.addElement(element);
		}

		void clearElements() override
		{
			container.clearElements();
		}
	private:
		sf::Vector2f position;

		const Style style;

		void onAddElement()
		{
			float percent = container.getSize().y / style.size.y;

			if (percent > 1.f)
			{
				percent = 1.f;
			}

			scrollBar.setConsumption(percent);
		}

		void onScrollBarMoved()
		{
			container.setSpecificOffset(sf::Vector2f(
				0,
				scrollBar.getCurrentPosition()
			));
		}

		class Container
			:
			public SimpleContainer
		{
		public:
			Container(
				ElementBase* const parent,
				const SimpleContainer::Style style,
				const SimpleContainer::Direction direction,
				VerticalScrollContainer* const container)
				:
				SimpleContainer(
					parent,
					style,
					direction),
				container(container)
			{
			}

			void addElement(
				ElementBase* const element) override
			{
				SimpleContainer::addElement(element);

				container->onAddElement();
			}

		private:
			VerticalScrollContainer* const container;
		} container;

		class ScrollBar
			:
			public SimpleScrollBar
		{
		public:
			ScrollBar(
				VerticalScrollContainer* const parent,
				const Style style,
				const Direction direction,
				const sf::View* const view,
				VerticalScrollContainer* const container)
				:
				SimpleScrollBar(
					parent,
					style,
					direction,
					view),
				container(container)
			{
			}

			void onLogic(
				const sf::Time time) override { }

			void onScrollBarMoved() override
			{
				SimpleScrollBar::onScrollBarMoved();

				container->onScrollBarMoved();
			}
		private:
			VerticalScrollContainer* const container;
		} scrollBar;
};
}
