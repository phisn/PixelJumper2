#pragma once

#include <Client/source/menu/MenuRootBase.h>
#include <Client/source/menu/MenuScrollContainer.h>

#include <Client/source/editor/tilechoice/TileChoiceWorker.h>

#include <Client/source/editor/manipulator/EditorCache.h>
#include <Client/source/editor/manipulator/Manipulator.h>

namespace Editor
{
	class TileBarMaterial
		:
		public Menu::BarMaterial::DefaultRectangle
	{
	public:
		using DefaultRectangle::DefaultRectangle;
	};

	class TileSliderMaterial
		:
		public Menu::SliderMaterial::DefaultRectangle
	{
	public:
		using DefaultRectangle::DefaultRectangle;
	};

	class TileChoiceRoot
		:
		public Menu::MenuRootBase
	{
		typedef Menu::ScrollContainer<TileBarMaterial, TileSliderMaterial> ScrollContainer;
	public:
		TileChoiceRoot()
			:
			scrollContainer(Menu::CommonControlDirection::Vertical)
		{
		}

		bool build() override
		{
			addChild(&scrollContainer);

			scrollContainer.position = { 0, 0 };
			scrollContainer.slideBarWidth = 15.f;

			size.addListener([this](
				const sf::Vector2f oldSize,
				const sf::Vector2f newSize)
			{
				scrollContainer.sizePreferred = newSize;
				scrollContainer.updateGraphics();
			});

			Manipulator::GetCache()->tileChoice.addWorker(
				std::ref(worker), 0
			);

			Manipulator::GetCache()->tileSearch.addListener(
				[this](const TileSearchOutput* const output) -> void
			{
				scrollContainer.removeAllElements();

				for (TileTemplate* const tile : output->tiles)
				{
					scrollContainer.addElement(tile->createRepresentation());
				}

				scrollContainer.updateGraphics();
			});
			scrollContainer.sizePreferred = *size;

			return true;
		}

	private:
		ScrollContainer scrollContainer;
		TileChoiceWorker worker;
	};
}
