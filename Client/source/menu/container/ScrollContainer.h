#pragma once

#include <Client/source/menu/Common.h>

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
		public ContainerBase
	{
	public:
		struct Properties
			:
			public ContainerBase::Properties
		{
			ElementBase::Properties
				* scrollBar,
				* rowContainer;
		};

		ScrollContainer(
			const MENU::Direction direction)
			:
			container(direction),
			scrollBar(direction)
		{
		}

		void initialize(
			ElementBase::Properties* const EB_properties) override
		{
			ContainerBase::initialize(EB_properties);
			Properties* properties = (Properties*) EB_properties;

			container.initialize(properties->rowContainer);
			scrollBar.initialize(properties->scrollBar);
		}

	private:
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
				const MENU::Direction direction)
				:
				ScrollBarPolicy(direction)
			{

			}

			void onScrollBarMoved() override
			{
			}
		} scrollBar;
	};
}
