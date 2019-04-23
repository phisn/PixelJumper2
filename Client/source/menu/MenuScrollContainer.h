#pragma once

#include <Client/source/menu/MenuRowContainer.h>
#include <Client/source/menu/MenuSlideBar.h>

namespace Menu
{
	template <
		typename TBarMaterial,
		typename TSliderMaterial>
	class ScrollContainer
		:
		public ElementBase
	{
		typedef SlideBar<TBarMaterial, TSliderMaterial> TSlideBar;
		typedef ScrollContainer<TBarMaterial, TSliderMaterial> TThis;

		class CustomSlideBar : public TSlideBar
		{
		public:
			CustomSlideBar(
				TThis* const parent,
				const CommonControlDirection direction)
				:
				parent(parent),
				SlideBar(direction)
			{
			}

			void onSliderPressed(const float distance) override
			{
			}

			void onSliderMoved(const float distance) override
			{
				parent->onSliderMoved(distance);
			}

		private:
			TThis* const parent;
		};
	public:
		CustomSlideBar slideBar;

		ScrollContainer(const CommonControlDirection direction)
			:
			direction(direction),
			container(direction),
			slideBar(this, direction)
		{
			ElementBase::addStaticChild(&slideBar);
			ElementBase::addStaticChild(&container);

			slideBar.size.addListener(
				[this](const sf::Vector2f oldSize,
					   const sf::Vector2f newSize)
				{
					container.position =
					{
						this->direction == CommonControlDirection::Horizontal
						? 0
						: newSize.x,
						this->direction == CommonControlDirection::Horizontal
						? newSize.y
						: 0
					};
				});

			slideBar.length->setPercent(1.f);
		}

		void addElement(
			ElementBase* const element,
			const size_t position)
		{
			container.addElement(element, position);
			updateSliderConsumption();
		}

		void addElement(ElementBase* const element)
		{
			container.addElement(element);
			updateSliderConsumption();
		}

		void removeElement(
			const size_t position)
		{
			container.removeElement(position);
			updateSliderConsumption();
		}

		void removeElement(
			const ElementBase* const element)
		{
			container.removeElement(element);
			updateSliderConsumption();
		}

		void removeAllElements()
		{
			slideBar.consumption->setPercent(1.f);
		}

		const Container& getElements() const
		{
			return ElementBase::getStaticChildren();
		}

	protected:
		virtual void updateSliderConsumption()
		{
			const float consumption = direction == CommonControlDirection::Horizontal
				? (size.getValue().x / container.size.getValue().x)
				: (size.getValue().y / container.size.getValue().y);

			slideBar.length->setPercent(
				consumption < 1.f ? consumption : 1.f
			);
		}

		virtual void onSliderMoved(const float distance)
		{
			container.position =
			{
				direction == CommonControlDirection::Horizontal
				? -distance
				: 0,
				direction == CommonControlDirection::Horizontal
				? 0
				: -distance
			};
		}

	private:
		const CommonControlDirection direction;

		RowContainer container;

		virtual void updateOwnGraphics() override
		{
		}
	};
}
