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

		virtual void onEvent(sf::Event event)
		{
			event = convertWindowEvent(event);

			switch (event.type)
			{
			case sf::Event::EventType::MouseButtonPressed:
				if (sf::FloatRect{
						0,
						0,
						renderSize.x,
						renderSize.y
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
//						primaryView.setCenter(renderTexture.mapPixelToCoords({
	//						event.mouseButton.x, event.mouseButton.y }, primaryView));

						primaryView.setCenter({ 0, 0 });

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
						0,
						0,
						windowSize.x,
						windowSize.y
					}.contains(event.mouseWheelScroll.x, event.mouseWheelScroll.y))
				{
					sf::Vector2f newCenter = renderTexture.mapPixelToCoords({
						event.mouseWheelScroll.x, event.mouseWheelScroll.y });
					primaryView.zoom(exp2f(event.mouseWheelScroll.delta));
					primaryView.setCenter(newCenter);
				}

				break;
			}
		}

		int mm_x, mm_y;

	protected:
		float GridSize = 50.f;

		sf::Color BackgroundColor = { 40, 40, 40 };
		sf::Color BackgroundGridRootColor = { 160, 160, 160 };
		sf::Color BackgroundGridColor = { 80, 80, 80 };

		sf::View primaryView;

	private:
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
				), primaryView);

			sf::Vector2f mouseBeginPosition = renderTexture.mapPixelToCoords(
				mouseBegin, primaryView);

			return mouseBeginPosition - mouseGridPosition;
		}

		// grid representation
	private:
		sf::Vector2u oldTargetSize{ };
		sf::VertexArray grid;
		bool gridNeedUpdate = true;

		virtual void onDraw(sf::RenderTarget* target) override
		{
			ImGui::Begin("pos");
			target->setView(primaryView);
			ImGui::Text("pos: x(%d) y(%d)", mm_x, mm_y);

			sf::Vector2f p1 = target->mapPixelToCoords({ mm_x, mm_y }, primaryView);
			ImGui::Text("p1: x(%.4f) y(%.4f)", p1.x, p1.y);
			/*
			sf::Vector2f p3 = target->mapPixelToCoords({ (int)p2.x, (int)p2.y }, primaryView);
			ImGui::Text("p3: x(%.4f) y(%.4f)", p3.x, p3.y);*/
			ImGui::Text("center: x(%.4f) y(%.4f)", primaryView.getCenter().x, primaryView.getCenter().y);
			ImGui::Text("size: x(%.4f) y(%.4f)", primaryView.getSize().x, primaryView.getSize().y);
			ImGui::Text("renderpos: x(%.4f) y(%.4f)", renderPosition.x, renderPosition.y);
			ImGui::Text("rendersize: x(%.4f) y(%.4f)", renderSize.x, renderSize.y);
			ImGui::Text("pos: x(%.4f) y(%.4f)", windowPosition.x, windowPosition.y);
			ImGui::Text("size: x(%.4f) y(%.4f)", windowSize.x, windowSize.y);

			ImGui::End();

			if (oldTargetSize != target->getSize())
			{
				if (oldTargetSize != sf::Vector2u{ 0, 0 })
				{
					sf::Vector2f center = primaryView.getCenter();
					primaryView.setSize(
						primaryView.getSize().x * (float) target->getSize().x / (float) oldTargetSize.x,
						primaryView.getSize().y * (float) target->getSize().y / (float) oldTargetSize.y);
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

		void _process() override
		{
			sf::Vector2f p2 = Device::Screen::GetWindow()->mapPixelToCoords({ mm_x, mm_y }, primaryView);
			ImGui::Text("p2: x(%.4f) y(%.4f)", p2.x, p2.y);

		}

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