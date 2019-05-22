#pragma once

#include <Client/source/menu/MenuRootBase.h>

#include <Client/source/editor/manipulator/EditorCache.h>
#include <Client/source/editor/manipulator/Manipulator.h>


#include <Client/source/menu/MenuScrollContainer.h>

namespace Editor
{
	class TileBarMaterial
		:
		public Menu::BarMaterial::DefaultRectangle
	{
	};

	class TileSliderMaterial
		:
		public Menu::SliderMaterial::DefaultRectangle
	{
	};

	class TileChoiceRoot
		:
		public Menu::MenuRootBase
	{
		typedef Menu::ScrollContainer<TileBarMaterial, TileSliderMaterial> ScrollContainer;
	public:
		bool build() override
		{
			addStaticChild(&scrollContainer);

			scrollContainer.position = { 0, 0 };
			scrollContainer.slideBarWidth = 10.f;

			size.addListener([this](
				const sf::Vector2f oldSize,
				const sf::Vector2f newSize)
			{
				scrollContainer.size = newSize;
			});

			Manipulator::GetCache()->tileChoice.addListener(
				[this](const TileChoiceOutput* const output) -> void
			{
				scrollContainer.removeAllElements();

				for (TileTemplate* const tile : output->tiles)
				{
					scrollContainer.addElement(tile->createRepresentation());
				}
			});
		}

	private:
		ScrollContainer scrollContainer;
	};
}
