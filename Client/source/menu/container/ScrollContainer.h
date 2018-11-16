#pragma once

#include <Client/source/menu/Common.h>

#include <Client/source/menu/container/RowContainerBase.h>
#include <Client/source/menu/element/ScrollBarBase.h>

#ifdef __INTELLISENSE__
#include <Client/source/menu/style/DefaultStyles.h>
#endif

namespace Menu
{
	template <
		class ScrollBarPolicy,
		class RowContainerPolicy
	>
	class ScrollContainer
		:
		public ContainerBase<RowContainerBase::Iterator>
	{
		const Direction direction;
	public:
		struct Properties
			:
			public ContainerBase<RowContainerBase::Iterator>::Properties
		{
			ElementBase::Properties
				* scrollBar,
				* rowContainer;
		};

		ScrollContainer(
			const MENU::Direction direction)
			:
			direction(direction),

			container(direction),
			scrollBar(this, direction) // callback
		{
		}

		void initialize(
			ElementBase::Properties* const EB_properties) override
		{
			
			Properties* properties = (Properties*)EB_properties;

			container.initialize(properties->rowContainer);
			scrollBar.initialize(properties->scrollBar);

			ContainerBase::initialize(EB_properties);
			
			properties->scrollBar->position += properties->position;
			properties->rowContainer->position += properties->position;

			container.setParent(this);
			scrollBar.setParent(this);
		}

		void onEvent(
			const sf::Event event) override
		{
			if (container.isUseOnEvent())
			{
				container.onEvent(event);
			}

			if (scrollBar.isUseOnEvent())
			{
				scrollBar.onEvent(event);
			}
		}

		void onLogic(
			const sf::Time time) override
		{
			if (container.isUseOnLogic())
			{
				container.onLogic(time);
			}

			if (scrollBar.isUseOnLogic())
			{
				scrollBar.onLogic(time);
			}
		}
		
		void onDraw() override
		{
			container.onDraw();
			scrollBar.onDraw();
		}

		void resetPosition() override
		{
			ContainerBase::resetPosition();

			container.resetPosition();
			scrollBar.resetPosition();

			resetConsumption();
		}
		
		void addElement(
			ElementBase* const element) override
		{
			container.addElement(element);

			consumedLength += direction == Direction::Horizontal
				? element->getSize().x
				: element->getSize().y;
			updateConsumption();
		}
		
		void clearElements() override
		{
			container.clearElements();

			consumedLength = 0.f;
			updateConsumption();
		}

		Iterator begin() override
		{
			return container.begin();
		}

		Iterator end() override
		{
			return container.end();
		}
		
		bool isEmpty() const override
		{
			return container.isEmpty();
		}

	private:
		float consumedLength = 0.f;

		void resetConsumption()
		{
			consumedLength = 0.f;

			for (Iterator it = container.begin(); it != container.end(); ++it)
			{
				ElementBase* element = *it;

				consumedLength += direction == Direction::Horizontal
					? element->getSize().x
					: element->getSize().y;
			}

			updateConsumption();
		}

		void updateConsumption()
		{
			float consumption = consumedLength / (direction == Direction::Horizontal
				? container.getSize().x
				: container.getSize().y);

			if (consumption > 1.f)
			{
				consumption = 1.f;
			}

			scrollBar.setConsumption(consumption);
		}

		void onScrollBarMoved()
		{
			container.setSpecificOffset(sf::Vector2f(
				direction == Direction::Horizontal
					? scrollBar.getOffset()
					: container.getPosition().x,
				direction == Direction::Vertical
					? scrollBar.getOffset()
					: container.getPosition().y
			));
		}

		class Container
			:
			public RowContainerPolicy
		{
		public:
			Container(
				const MENU::Direction direction)
				:
				RowContainerPolicy(direction)
			{
			}

		} container;

		class ScrollBar
			:
			public ScrollBarPolicy
		{
		public:
			ScrollBar(
				ScrollContainer* container,
				const MENU::Direction direction)
				:
				container(container),
				ScrollBarPolicy(direction)
			{
			}

			void onScrollBarMoved() override
			{
				container->onScrollBarMoved();
			}

		private:
			ScrollContainer* container;

		} scrollBar;
	};
}
