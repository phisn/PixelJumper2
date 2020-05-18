#pragma once

#include "FrameworkCore/imgui/ImGuiRenderWindow.h"
#include "FrameworkCore/ViewChain.h"

namespace Editor
{
	class ClassicWorldWindow
		:
		public Framework::ImGuiRenderWindow
	{
		const float GridSize = 50.f;
		const sf::Color BackgroundColor = { 40, 40, 40 };
		const sf::Color BackgroundGridRootColor = { 160, 160, 160 };
		const sf::Color BackgroundGridColor = { 80, 80, 80 };

	public:
		ClassicWorldWindow()
		{
			setTitle("World Window #n");
			title = "ClassicContext";
			grid.setPrimitiveType(sf::PrimitiveType::Lines);
			primaryView.setSize({ 100, 100 });
			primaryView.setPosition({ 0, 0 });

			sf::View view;
		}

		void onEvent(sf::Event event)
		{
			convertWindowEvent(event);

			switch (event.type)
			{
			case sf::Event::EventType::MouseButtonPressed:
				if (sf::FloatRect{ 
						windowPosition.x,
						windowPosition.y,
						windowSize.x,
						windowSize.y
					}.contains(event.mouseButton.x, event.mouseButton.y))
				{
					switch (event.mouseButton.button)
					{
					case sf::Mouse::Button::Left:
						mousePressed = true;

						mouseBegin = sf::Vector2i{ event.mouseButton.x, event.mouseButton.y };
						mouseGridBegin = primaryView.getCenter();

						break;
					case sf::Mouse::Button::Middle:
						primaryView.setCenter(renderTexture.mapPixelToCoords({
							event.mouseButton.x, event.mouseButton.y }, primaryView.getView()));

						gridNeedUpdate = true;
						break;
					}
				}

				break;
			case sf::Event::EventType::MouseButtonReleased:
				if (mousePressed && event.mouseButton.button == sf::Mouse::Button::Left)
				{
					mousePressed = false;

					primaryView.setCenter(mouseGridBegin + makeMouseGridOffset(
						event.mouseButton.x,
						event.mouseButton.y));

					gridNeedUpdate = true;
				}

				break;
			case sf::Event::EventType::MouseMoved:
				mm_x = event.mouseMove.x;
				mm_y = event.mouseMove.y;

				if (mousePressed)
				{
					primaryView.setCenter(mouseGridBegin + makeMouseGridOffset(
						event.mouseMove.x,
						event.mouseMove.y));

					gridNeedUpdate = true;
				}

				break;
			case sf::Event::EventType::MouseWheelScrolled:
				if (sf::FloatRect{
						windowPosition.x,
						windowPosition.y,
						windowSize.x,
						windowSize.y
					}.contains(event.mouseWheelScroll.x, event.mouseWheelScroll.y))
				{
					sf::Vector2f newCenter = renderTexture.mapPixelToCoords({		
						event.mouseWheelScroll.x, event.mouseWheelScroll.y});
					primaryView.zoom(exp2f(event.mouseWheelScroll.delta));
					primaryView.setCenter(newCenter);
				}

				break;
			}
		}

		int mm_x, mm_y;

	private:
		Device::View primaryView;
		sf::RenderTexture background;

		// grid movement
	private:
		bool mousePressed = false;
		sf::Vector2i mouseBegin;
		sf::Vector2f mouseGridBegin;

		sf::Vector2f makeMouseGridOffset(int mouse_x, int mouse_y)
		{
			sf::Vector2f mouseGridPosition = renderTexture.mapPixelToCoords(
				sf::Vector2i(
					mouse_x,
					mouse_y
				), primaryView.getView());

			sf::Vector2f mouseBeginPosition = renderTexture.mapPixelToCoords(
				mouseBegin, primaryView.getView());

			return
			{
				mouseBeginPosition.x - mouseGridPosition.x,
				mouseGridPosition.y - mouseBeginPosition.y
			};
		}

		// grid representation
	private:
		sf::Vector2u oldTargetSize{ };
		sf::VertexArray grid;
		bool gridNeedUpdate = true;

		void onDraw(sf::RenderTarget* target) override
		{
			ImGui::Begin("pos");

			primaryView.enable(target);
			sf::Vector2f p1 = target->mapPixelToCoords({ mm_x, mm_y }, primaryView.getView());
			ImGui::Text("p1: x(%.4f) y(%.4f)", p1.x, p1.y);

			sf::Vector2f p2 = Device::Screen::GetWindow()->mapPixelToCoords({ mm_x, mm_y }, primaryView.getView());
			ImGui::Text("p2: x(%.4f) y(%.4f)", p2.x, p2.y);

			primaryView.enable(target);
			sf::Vector2f p3 = target->mapPixelToCoords({ (int) p2.x, (int) p2.y }, primaryView.getView());
			ImGui::Text("p3: x(%.4f) y(%.4f)", p3.x, p3.y);

			
			ImGui::End();

			if (oldTargetSize != target->getSize())
			{
				if (oldTargetSize != sf::Vector2u{ 0, 0 })
				{
					sf::Vector2f position = primaryView.getCenter();
					position.x -= primaryView.getSize().x / 2.f;
					position.y += primaryView.getSize().y / 2.f;

					primaryView.setSize(
						primaryView.getSize().x * (float) target->getSize().x / (float) oldTargetSize.x,
						primaryView.getSize().y * (float) target->getSize().y / (float) oldTargetSize.y);
					
					position.x += primaryView.getSize().x / 2.f;
					position.y -= primaryView.getSize().y / 2.f;

					primaryView.setCenter(position);
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

		void updateGrid()
		{
			grid.resize(calculateGridVertexCount());
			int gridIndex = 0;

			sf::FloatRect viewRect = primaryView.getRect();
			for (float pure_x = 0; pure_x < primaryView.getSize().x; pure_x += GridSize)
			{
				float x = pure_x + viewRect.left + GridSize / 2.f - fmodf(viewRect.left, GridSize);
				
				sf::Vertex* vertex = &grid[gridIndex++];
				vertex->position = { x, viewRect.top - GridSize };
				vertex->color = BackgroundGridColor;

				vertex = &grid[gridIndex++];
				vertex->position = { x, viewRect.top + viewRect.height };
				vertex->color = BackgroundGridColor;
			}

			for (float pure_y = 0; pure_y < primaryView.getSize().y; pure_y += GridSize)
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
			return 4 + std::ceil(primaryView.getSize().x / GridSize) * 2 + std::ceil(primaryView.getSize().y / GridSize) * 2;
		}
	};
}