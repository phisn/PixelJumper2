#pragma once

#include <Client/source/editor/manipulator/EditorCache.h>
#include <Client/source/editor/manipulator/Manipulator.h>

#include <Client/source/editor/selector/SelectorCache.h>
#include <Client/source/editor/selector/SelectorView.h>

#include <SFML/Graphics.hpp>

namespace Editor
{
	class SelectorConverter
	{
	public:
		bool operator()(
			const SelectorInput* const input,
			SelectorOutput* const output)
		{
			this->input = input;
			this->output = output;

			constructArea();
			collectTiles();

			return true;
		}

	private:
		void constructArea()
		{
			const sf::Vector2f ltCorner =
			{
				input->beginCorner.x > input->nextCorner.x ? input->nextCorner.x : input->beginCorner.x,
				input->beginCorner.y > input->nextCorner.y ? input->nextCorner.y : input->beginCorner.y
			};
			const sf::Vector2f rbCorner =
			{
				input->beginCorner.x <= input->nextCorner.x ? input->nextCorner.x : input->beginCorner.x,
				input->beginCorner.y <= input->nextCorner.y ? input->nextCorner.y : input->beginCorner.y
			};

			output->area.left = ltCorner.x - fmod(ltCorner.x, SelectorView::GridRectSize) - (ltCorner.x < 0 ? SelectorView::GridRectSize : 0);
			output->area.top = ltCorner.y - fmod(ltCorner.y, SelectorView::GridRectSize) - (ltCorner.y < 0 ? SelectorView::GridRectSize : 0);
			
			const sf::Vector2f realNextCorner = sf::Vector2f(
				rbCorner.x - fmod(rbCorner.x, SelectorView::GridRectSize) - (rbCorner.x < 0 ? SelectorView::GridRectSize : 0) + SelectorView::GridRectSize,
				rbCorner.y - fmod(rbCorner.y, SelectorView::GridRectSize) - (rbCorner.y < 0 ? SelectorView::GridRectSize : 0) + SelectorView::GridRectSize
			);

			output->area.width = realNextCorner.x - output->area.left;
			output->area.height = realNextCorner.y - output->area.top;

			output->type = SelectorType::Area;
		}

		void collectTiles()
		{
			if (false)
			{
				output->type = SelectorType::Tiles;
			}
		}

		const SelectorInput* input;
		SelectorOutput* output;

		SelectorType oldSelectorType = (SelectorType) -1;
	};

	class SelectorMarker
	{
	public:
		SelectorMarker(const sf::View& view)
			:
			view(view)
		{
			Manipulator::GetCache()->selection.addListener(
				std::ref(*this)
			);
			Manipulator::GetCache()->selection.addWorker(
				std::ref(converter),
				DefaultConverterPriority
			);
		}

		void operator()(const SelectorOutput* const output)
		{
			tileMarker.setFillColor(sf::Color::Color(100, 100, 100, 100));
			tileMarker.setPosition(
				output->area.left,
				output->area.top);
			tileMarker.setSize(sf::Vector2f(
				output->area.width,
				output->area.height));
		}

		void beginMouseMovement(const sf::Vector2i beginPosition)
		{
			beginCoordsPosition = Device::Screen::GetWindow()->mapPixelToCoords(beginPosition, view);
		}

		void nextMouseMovement(const sf::Vector2i nextPosition)
		{
			const sf::Vector2f nextCoordsPosition =
				Device::Screen::GetWindow()->mapPixelToCoords(nextPosition, view);

			Manipulator::GetCache()->selection.writeInput()->beginCorner =
			{
				beginCoordsPosition.x,
				beginCoordsPosition.y,
			};
			Manipulator::GetCache()->selection.writeInput()->nextCorner =
			{
				nextCoordsPosition.x,
				nextCoordsPosition.y
			};
			Manipulator::GetCache()->selection.notify(); // ignore return
		}

		void draw() const
		{
			Device::Screen::Draw(tileMarker);
		}

	private:
		sf::RectangleShape outlineMarker, tileMarker;

		const sf::View& view;
		sf::Vector2f beginCoordsPosition;

		SelectorConverter converter;
	};
}