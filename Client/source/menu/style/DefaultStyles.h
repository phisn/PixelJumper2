#pragma once

#include <Client/source/menu/container/SimpleRowContainer.h>
#include <Client/source/menu/element/SimpleScrollBar.h>

namespace Menu
{
	namespace DefaultStyle
	{
		class ScrollBar
			:
			public SimpleScrollBar
		{
		public:
			ScrollBar(
				ElementBase* const parent,
				const Direction direction,
				const sf::View* const view)
				:
				SimpleScrollBar(
					parent,
					style,
					direction,
					view)
			{
			}

			void onLogic(
				const sf::Time) override { }
		private:
			static Style style;
		};

		class RowContainer
			:
			public SimpleRowContainer
		{
		public:
			RowContainer(
				ElementBase* const parent,
				const Direction direction)
				:
				SimpleRowContainer(
					parent,
					style,
					direction)
			{
			}

		private:
			const Style style = 
			{
				0.0f // margin
			};
		};
	}
}