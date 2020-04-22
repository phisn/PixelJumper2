#pragma once

#include "FrameworkCore/imgui/ImGuiRenderWindow.h"
#include "FrameworkCore/ViewChain.h"

namespace Editor
{
	class ClassicContextWindow
		:
		public Framework::ImGuiRenderWindow
	{
		const float GridSize = 50.f;
		const sf::Color BackgroundColor = { 40, 40, 40 };
		const sf::Color BackgroundGridColor = { 80, 80, 80 };

	public:
		ClassicContextWindow()
		{
			title = "ClassicContext";
			grid.setPrimitiveType(sf::PrimitiveType::Lines);
			primaryView.setSize({ 100, 100 });
			primaryView.setPosition({ 0, 0 });
		}

	private:
		sf::VertexArray grid;
		bool gridNeedUpdate = true;

		Device::View primaryView;
		sf::RenderTexture background;

		sf::Vector2u oldTargetSize{ };

		void onDraw(sf::RenderTarget* target) override
		{
			ImGui::Begin("a");
			if (ImGui::Button("push"))
			{
				primaryView.setCenter(primaryView.getCenter() + sf::Vector2f(10, 10));
			}

			ImGui::Text("center: x{%.4f} y{%.4f}", primaryView.getCenter().x, primaryView.getCenter().y);
			ImGui::Text("position: x{%.4f} y{%.4f}", primaryView.getPosition().x, primaryView.getPosition().y);
			ImGui::Text("tlpos: x{%.4f} y{%.4f}", primaryView.getCenter().x - primaryView.getSize().x / 2.f, primaryView.getCenter().y + primaryView.getSize().y / 2.f);
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
			}

			primaryView.enable(target);
			target->clear(BackgroundColor);

			if (true) // gridNeedUpdate || calculateGridVertexCount() != grid.getVertexCount()
			{
				Log::Information(L"remove this later | grid update");
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
				float x = pure_x + viewRect.left + GridSize / 2.f + fmodf(viewRect.left, GridSize);
				
				sf::Vertex* vertex = &grid[gridIndex++];
				vertex->position = { x, viewRect.top - GridSize };
				vertex->color = BackgroundGridColor;

				vertex = &grid[gridIndex++];
				vertex->position = { x, viewRect.top + viewRect.height };
				vertex->color = BackgroundGridColor;
			}

			for (float pure_y = 0; pure_y < primaryView.getSize().y; pure_y += GridSize)
			{
				float y = pure_y + viewRect.top + GridSize / 2.f + fmodf(viewRect.height, GridSize);

				sf::Vertex* vertex = &grid[gridIndex++];
				vertex->position = { viewRect.left - GridSize, y };
				vertex->color = BackgroundGridColor;

				vertex = &grid[gridIndex++];
				vertex->position = { viewRect.left + viewRect.width, y };
				vertex->color = BackgroundGridColor;
			}

			assert(gridIndex == grid.getVertexCount());
		}

		int calculateGridVertexCount()
		{
			return std::ceil(primaryView.getSize().x / GridSize) * 2 + std::ceil(primaryView.getSize().y / GridSize) * 2;
		}
	};
}