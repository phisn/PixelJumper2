#pragma once

#include "FrameworkCore/imgui/ImGuiRenderWindow.h"
#include "FrameworkCore/ViewChain.h"

namespace Framework
{
	class ImGuiGridWindow
		:
		public ImGuiRenderWindow
	{
	public:
		ImGuiGridWindow()
			:
			ImGuiRenderWindow(WindowFlagsDefault)
		{
			grid.setPrimitiveType(sf::PrimitiveType::Lines);
		}

		virtual void onEvent(sf::Event& event)
		{
			convertWindowEvent(event);

			switch (event.type)
			{
			case sf::Event::EventType::MouseButtonPressed:
				if (affectsWindow(event.mouseButton.x, event.mouseButton.y))
				{
					switch (event.mouseButton.button)
					{
					case sf::Mouse::Button::Right:
						mousePressed = true;
						mouseMoved = false;

						mouseBegin = sf::Vector2i{ event.mouseButton.x, event.mouseButton.y };
						mouseGridBegin = primaryView.getCenter();

						break;
					case sf::Mouse::Button::Middle:
						primaryView.setCenter({ 0, 0 });
						gridNeedUpdate = true;

						break;
					}
				}

				break;
			case sf::Event::EventType::MouseButtonReleased:
				if (mousePressed && event.mouseButton.button == sf::Mouse::Button::Right)
				{
					if (!mouseMoved
						&& event.mouseButton.x == mouseBegin.x
						&& event.mouseButton.y == mouseBegin.y)
					{
						onMovementClick(this->pixelToCoords(event.mouseButton.x, event.mouseButton.y));
					}

					mousePressed = false;
					primaryView.setCenter(mouseGridBegin + makeMouseGridOffset(
						mouseBegin,
						event.mouseButton.x,
						event.mouseButton.y));
					gridNeedUpdate = true;
				}

				break;
			case sf::Event::EventType::MouseMoved:
				if (mousePressed)
				{
					mouseMoved = true;

					primaryView.setCenter(mouseGridBegin + makeMouseGridOffset(
						mouseBegin,
						event.mouseMove.x,
						event.mouseMove.y));

					gridNeedUpdate = true;
				}

				break;
			case sf::Event::EventType::MouseWheelScrolled:
				if (affectsWindow(event.mouseWheelScroll.x, event.mouseWheelScroll.y))
				{
					if (event.mouseWheelScroll.delta > 0)
					{
						if (std::min(primaryView.getSize().x / renderTexture.getSize().x, primaryView.getSize().y / renderTexture.getSize().y) < minViewSize)
							break;
					}
					else
					{
						if (std::max(primaryView.getSize().x / renderTexture.getSize().x, primaryView.getSize().y / renderTexture.getSize().y) > maxViewSize)
							break;
					}

					sf::Vector2f mousePtr = renderTexture.mapPixelToCoords({
						event.mouseWheelScroll.x, event.mouseWheelScroll.y }, primaryView);

					primaryView.zoom(powf(0.9f, event.mouseWheelScroll.delta));

					sf::Vector2f zoomedMousePtr = renderTexture.mapPixelToCoords({
						event.mouseWheelScroll.x, event.mouseWheelScroll.y }, primaryView);

					primaryView.move(mousePtr - zoomedMousePtr);

					gridNeedUpdate = true;
				}

				break;
			}
		}

	protected:
		float zoomSpeed = 0.9f;
		float maxViewSize = 10.f;
		float minViewSize = 0.4f;
		float GridSize = 50.f;

		sf::Color BackgroundColor = { 40, 40, 40 };
		sf::Color BackgroundGridRootColor = { 160, 160, 160 };
		sf::Color BackgroundGridColor = { 80, 80, 80 };

		sf::View primaryView;

		virtual void onDraw(sf::RenderTarget* target) override
		{
			target->setView(primaryView);
			if (oldTargetSize != target->getSize())
			{
				// prevent grid moving while resizing
				mousePressed = false;

				if (oldTargetSize != sf::Vector2u{ 0, 0 })
				{
					sf::Vector2f center = primaryView.getCenter();
					primaryView.setSize(
						primaryView.getSize().x * (float)target->getSize().x / (float)oldTargetSize.x,
						primaryView.getSize().y * (float)target->getSize().y / (float)oldTargetSize.y);
					primaryView.setCenter(center);
				}
				else
				{
					primaryView.setSize(sf::Vector2f(target->getSize()));
				}

				oldTargetSize = target->getSize();
				gridNeedUpdate = true;
			}
			
			target->clear(BackgroundColor);

			if (gridNeedUpdate || calculateGridVertexCount() != grid.getVertexCount())
			{
				updateGrid();

				gridNeedUpdate = false;
			}

			target->draw(grid);
		}

		virtual void onMovementClick(sf::Vector2f point)
		{
		}

		sf::Vector2f pixelToCoords(int x, int y) const
		{
			return renderTexture.mapPixelToCoords({ x, y }, primaryView);
		}

		sf::Vector2f makeMouseGridOffset(sf::Vector2i begin, int mouse_x, int mouse_y) const
		{
			sf::Vector2f mouseGridPosition = renderTexture.mapPixelToCoords(
				sf::Vector2i(
					mouse_x,
					mouse_y
				), primaryView);

			sf::Vector2f mouseBeginPosition = renderTexture.mapPixelToCoords(
				begin, primaryView);

			return mouseBeginPosition - mouseGridPosition;
		}

	private:
		sf::RenderTexture background;

		// grid movement
	private:
		bool mousePressed = false;
		bool mouseMoved;

		sf::Vector2i mouseBegin;
		sf::Vector2f mouseGridBegin;

		// grid representation
	private:
		sf::Vector2u oldTargetSize{ };
		sf::VertexArray grid;
		bool gridNeedUpdate = true;

		void updateGrid()
		{
			grid.resize(calculateGridVertexCount());
			int gridIndex = 0;

			sf::FloatRect viewRect =
			{
				primaryView.getCenter().x - primaryView.getSize().x / 2.f,
				primaryView.getCenter().y - primaryView.getSize().y / 2.f,
				primaryView.getSize().x,
				primaryView.getSize().y
			};

			for (float pure_x = -GridSize; pure_x < primaryView.getSize().x + GridSize; pure_x += GridSize)
			{
				float x = pure_x + viewRect.left + GridSize / 2.f - fmodf(viewRect.left, GridSize);

				sf::Vertex* vertex = &grid[gridIndex++];
				vertex->position = { x, viewRect.top - GridSize };
				vertex->color = BackgroundGridColor;

				vertex = &grid[gridIndex++];
				vertex->position = { x, viewRect.top + viewRect.height };
				vertex->color = BackgroundGridColor;
			}

			for (float pure_y = -GridSize; pure_y < primaryView.getSize().y + GridSize; pure_y += GridSize)
			{
				float y = pure_y + viewRect.top + GridSize / 2.f - fmodf(viewRect.top, GridSize);

				sf::Vertex* vertex = &grid[gridIndex++];
				vertex->position = { viewRect.left - GridSize, y };
				vertex->color = BackgroundGridColor;

				vertex = &grid[gridIndex++];
				vertex->position = { viewRect.left + viewRect.width, y };
				vertex->color = BackgroundGridColor;
			}

			sf::Vertex* vertex = &grid[gridIndex++];
			vertex->position = { viewRect.left - GridSize, 0 };
			vertex->color = BackgroundGridRootColor;

			vertex = &grid[gridIndex++];
			vertex->position = { viewRect.left + viewRect.width, 0 };
			vertex->color = BackgroundGridRootColor;

			vertex = &grid[gridIndex++];
			vertex->position = { 0, viewRect.top - GridSize };
			vertex->color = BackgroundGridRootColor;

			vertex = &grid[gridIndex++];
			vertex->position = { 0, viewRect.top + viewRect.height };
			vertex->color = BackgroundGridRootColor;

			assert(gridIndex == grid.getVertexCount());
		}

		int calculateGridVertexCount()
		{
			return 4 + 8 + std::ceil(primaryView.getSize().x / GridSize) * 2 + std::ceil(primaryView.getSize().y / GridSize) * 2;
		}
	};
}