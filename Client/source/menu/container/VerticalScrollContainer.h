#pragma once

#include <Client/source/menu/container/SimpleRowContainer.h>
#include <Client/source/menu/element/SimpleScrollBar.h>

#ifdef _DEBUG
#	ifndef USE_POLICIES
#	define HIDE_POLICIES
#	endif
#endif

#ifdef HIDE_POLICIES
#include <Client/source/menu/style/DefaultStyles.h>
#endif
#include <Client/source/menu/style/DefaultStyles.h>

namespace Menu
{
#ifndef HIDE_POLICIES
	template<
		class ScrollBarPolicy,
		class RowContainerPolicy
	>
#endif
	class VerticalScrollContainer
		:
		public ContainerBase
	{
	public:
#ifdef HIDE_POLICIES
		typedef DefaultStyle::ScrollBar ScrollBarPolicy;
		typedef DefaultStyle::RowContainer RowContainerPolicy;
#endif
		struct Properties
			:
			public ContainerBase::Properties
		{
			DefaultStyle::ScrollBar::Properties scrollBar;
			DefaultStyle::RowContainer::Properties container;
		};

		struct Style
		{
			float innerMargin;
		};

		VerticalScrollContainer(
			ElementBase* const parent,
			const sf::View* const view)
			:
			container(
				this, // maybe change later
				SimpleRowContainer::Direction::Vertical,
				this),
			scrollBar(

				this, // maybe change later
				SimpleScrollBar::Direction::Vertical,
				view,
				this),
			ContainerBase(
				parent)
		{
			useOnEvent = scrollBar.isUseOnEvent() || container.isUseOnEvent();
			useOnLogic = scrollBar.isUseOnLogic() || container.isUseOnLogic();
		}

		void initialize(
			ElementBase::Properties* const properties) override
		{
			this->properties = *(Properties*) properties;

			scrollBar.initialize(&this->properties.scrollBar);
			container.initialize(&this->properties.container);
		}

		void onEvent(
			const sf::Event event) override
		{
			if (scrollBar.isUseOnEvent())
			{
				scrollBar.onEvent(event);
			}

			if (container.isUseOnEvent())
			{
				container.onEvent(event);
			}
		}

		void onLogic(
			const sf::Time time) override
		{
			if (scrollBar.isUseOnLogic())
			{
				scrollBar.onLogic(time);
			}

			if (container.isUseOnLogic())
			{
				container.onLogic(time);
			}
		}

		void onDraw() override
		{
			scrollBar.onDraw();
			container.onDraw();
		}

		sf::Vector2f getSize() const override
		{
			return properties.size;
		}
		
		sf::Vector2f getPosition() const override
		{
			return position;
		}

		void loadPosition() override
		{
			position = convertPosition(
				properties.position);

			scrollBar.loadPosition();
			container.loadPosition();
		}

		void addElement(
			ElementBase* const element) override
		{
			container.addElement(element);

			useOnEvent |= element->isUseOnEvent();
			useOnLogic |= element->isUseOnLogic();
		}

		void clearElements() override
		{
			useOnEvent = scrollBar.isUseOnEvent() || container.isUseOnEvent();
			useOnLogic = scrollBar.isUseOnLogic() || container.isUseOnLogic();

			container.clearElements();
		}

		bool isEmpty() const override
		{
			return container.isEmpty();
		}

	private:
		sf::Vector2f position;
		Properties properties;

		void onAddElement()
		{
			float percent = container.getSize().y / properties.size.y;

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

		class RowContainer
			:
			public RowContainerPolicy
		{
		public:
			RowContainer(
				ElementBase* const parent,
				const SimpleRowContainer::Direction direction,

				VerticalScrollContainer* const container)
				:
				RowContainerPolicy(
					parent,
					direction),
				container(container)
			{
			}

			void addElement(
				ElementBase* const element) override
			{
				SimpleRowContainer::addElement(element);

				container->onAddElement();
			}

		private:
			VerticalScrollContainer* const container;
		} container;

		class ScrollBar
			:
			public ScrollBarPolicy
		{
		public:
			ScrollBar(
				VerticalScrollContainer* const parent,
				const SimpleScrollBar::Direction direction,
				const sf::View* const view,
				
				VerticalScrollContainer* const container)
				:
				ScrollBarPolicy(
					parent,
					direction,
					view),
				container(container)
			{
			}

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
