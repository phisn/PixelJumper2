#pragma once

#include <Client/source/menu/container/RowContainerBase.h>
#include <Client/source/menu/element/ScrollBarBase.h>

namespace
{
	const MENU::ScrollBarBase::Style scrollBarStyle =
	{
		sf::Color::Color(100, 100, 100),
		sf::Color::Color(50, 50, 50),

		sf::Color::Color(150, 150, 150),
		sf::Color::Color(50, 50, 50),

		sf::Color::Color(200, 200, 200),
		sf::Color::Color(50, 50, 50),
	};


	const MENU::RowContainerBase::Style rowContainerStyle =
	{
		0.0f
	};
}

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
					::scrollBarStyle,
					direction)
			{
			}

			void onLogic(
				const sf::Time) override { }
		private:
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
					::rowContainerStyle,
					direction)
			{
			}

		private:
		};
	}
}