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
	public:

		ScrollContainer(const CommonControlDirection direction)
			:
			direction(direction),
			container(direction),
			slideBar(this, direction)
		{
			ElementBase::addStaticChild(&slideBar);
			ElementBase::addStaticChild(&container);

			container.size.addListener(
				[this](const sf::Vector2f oldSize,
					   const sf::Vector2f newSize)
				{
					if (this->direction == CommonControlDirection::Horizontal)
					{
						size =
						{
							size.getValue().x,
							slideBarWidth.getValue() + newSize.y
						};
					}
					else
					{
						size =
						{
							slideBarWidth.getValue() + newSize.x,
							size.getValue().y
						};
					}

					updateSliderConsumption();
				});
			slideBarWidth.addListener(
				[this](const float oldWitdh,
					   const float newWidth)
				{
					if (this->direction == CommonControlDirection::Horizontal)
					{
						container.position = { 0, newWidth };
						container.size =
						{
							container.size.getValue().x,
							size.getValue().y - newWidth
						};
					}
					else
					{
						container.position = { newWidth, 0 };
						container.size =
						{
							size.getValue().x - newWidth,
							container.size.getValue().y
						};
					}
				});
			size.addListener(
				[this](const sf::Vector2f oldSize,
					   const sf::Vector2f newSize)
				{
					slideBar.size =
					{
						this->direction == CommonControlDirection::Horizontal
						? newSize.x
						: slideBarWidth.getValue(),
						this->direction == CommonControlDirection::Horizontal
						? slideBarWidth.getValue()
						: newSize.y
					};
				});

			slideBar.length->setPercent(1.f);
			slideBarWidth = 30.f;
		}

		void addElement(
			ElementBase* const element,
			const size_t position)
		{
			container.addElement(element, position);
		}

		void addElement(ElementBase* const element)
		{
			container.addElement(element);
		}

		void removeElement(
			const size_t position)
		{
			container.removeElement(position);
		}

		void removeElement(
			const ElementBase* const element)
		{
			container.removeElement(element);
		}

		void removeAllElements()
		{
			slideBar.consumption->setPercent(1.f);
		}

		const Container& getElements() const
		{
			return ElementBase::getStaticChildren();
		}

		Property<float> slideBarWidth;

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
				? -slideBar.distance->getPercent() * 
				(
					container.size.getValue().x - slideBar.size.getValue().x
				)
				: slideBarWidth.getValue(),
				direction == CommonControlDirection::Horizontal
				? slideBarWidth.getValue()
				: -slideBar.distance->getPercent() *
				(
					container.size.getValue().y - slideBar.size.getValue().y
				)
			};
		}

		virtual void updateOwnGraphics() override
		{
		}

	private:
		const CommonControlDirection direction;

		RowContainer container;

		void updateSlideBarSize()
		{
			if (direction == CommonControlDirection::Horizontal)
			{
				slideBar.size =
				{
					container.size.getValue().x,
					slideBarWidth
				};
			}
			else
			{
				slideBar.size =
				{
					slideBarWidth,
					container.size.getValue().y
				};
			}
		}

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

		CustomSlideBar slideBar;
	};
}
