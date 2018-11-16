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
			const ScrollBarBase::Style style = 
			{
				sf::Color::Color(100, 100, 100),
				sf::Color::Color(50, 50, 50),

				sf::Color::Color(150, 150, 150),
				sf::Color::Color(50, 50, 50),

				sf::Color::Color(200, 200, 200),
				sf::Color::Color(50, 50, 50),
			};
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
			const RowContainerBase::Style style =
			{
				0.0f
			};
		};
	}
}