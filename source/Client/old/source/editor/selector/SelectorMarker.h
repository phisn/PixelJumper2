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

			if (output->tiles.size() > 0)
			{
				output->type = SelectorType::Tiles;
			}
			else
			{
				output->type = SelectorType::Area;
			}

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
		}

		void collectTiles()
		{
			output->tiles.clear();

			
			for (Editor::TileBase* const tile : Manipulator::GetWorld()->getTiles())
			{
				const sf::Vector2f pos = tile->getPosition() * SelectorView::GridRectSize;
				const sf::Vector2f pospSize = pos + tile->getSize() * SelectorView::GridRectSize;

				sf::Vector2f lowCorner, highCorner;

				if (input->beginCorner.x > input->nextCorner.x)
				{
					lowCorner.x = input->nextCorner.x;
					highCorner.x = input->beginCorner.x;
				}
				else
				{
					lowCorner.x = input->beginCorner.x;
					highCorner.x = input->nextCorner.x;
				}

				if (input->beginCorner.y > input->nextCorner.y)
				{
					lowCorner.y = input->nextCorner.y;
					highCorner.y = input->beginCorner.y;
				}
				else
				{
					lowCorner.y = input->beginCorner.y;
					highCorner.y = input->nextCorner.y;
				}

				if (pospSize.x > lowCorner.x && pos.x < highCorner.x
				&&	pospSize.y > lowCorner.y && pos.y < highCorner.y)
				{
					output->tiles.push_back(tile);
				}
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
			if (multipleTileMarker.size() > 0)
			{
				multipleTileMarker.clear();
			}

			switch (output->type)
			{
			case SelectorType::Tiles:
				{
					for (TileBase* const tile : output->tiles)
					{
						sf::RectangleShape& shape = multipleTileMarker.emplace_back();

						shape.setFillColor(tile->getColor() - tileMarkerOffsetColor);
						shape.setPosition(tile->getPosition() * SelectorView::GridRectSize);
						shape.setSize(tile->getSize() * SelectorView::GridRectSize);
					}
				}

				break;
			case SelectorType::Area:
				{
					sf::RectangleShape& shape = multipleTileMarker.emplace_back();

					shape.setFillColor(tileMarkerColor);
					shape.setPosition(
						output->area.left,
						output->area.top);
					shape.setSize(sf::Vector2f(
						output->area.width,
						output->area.height));
				}

				break;
			};
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

		void draw(sf::RenderTarget* const target) const
		{
			for (const sf::RectangleShape& shape : multipleTileMarker)
				target->draw(shape);
		}

	private:
		const sf::Color tileMarkerColor = sf::Color::Color(100, 100, 100, 100);
		const sf::Color tileMarkerOffsetColor = sf::Color::Color(50, 50, 50, 0);

		sf::RectangleShape outlineMarker;
		std::vector<sf::RectangleShape> multipleTileMarker;

		const sf::View& view;
		sf::Vector2f beginCoordsPosition;

		SelectorConverter converter;
	};
}