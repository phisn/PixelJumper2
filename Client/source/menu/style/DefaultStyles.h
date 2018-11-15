#pragma once

#include <Client/source/menu/container/RowContainerBase.h>
#include <Client/source/menu/element/ScrollBarBase.h>

namespace Menu
{
	namespace DefaultStyle
	{
		template <
			class ScrollBarPolicy
		>
		class ScrollBar
			:
			public ScrollBarPolicy
		{
		public:
			ScrollBar(
				const Direction direction)
				:
				ScrollBarPolicy(
					style,
					direction)
			{
			}

			void onLogic(
				const sf::Time) override { }
		private:
			static ScrollBarBase::Style style;
		};

		template <
			class RowContainerPolicy
		>
		class RowContainer
			:
			public RowContainerPolicy
		{
		public:
			RowContainer(
				const Direction direction)
				:
				RowContainerPolicy(
					style,
					direction)
			{
			}

		private:
			static RowContainerBase::Style style;
		};
	}
}