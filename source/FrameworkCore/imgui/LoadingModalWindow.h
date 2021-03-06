#pragma once

#include "ImGuiModalWindow.h"

namespace Framework
{
	class LoadingModalWindow
		:
		public IndependentModalWindow
	{
	public:
		LoadingModalWindow(sf::Color dimColor = sf::Color::Color(0, 0, 0, 200))
			:
			IndependentModalWindow(WindowFlagsOverlay),
			dimColor(dimColor)
		{
			setTitle("loading_window");
		}

		using ModalWindow::open;
		using ModalWindow::close;

	private:
		sf::Color dimColor;

		void onContent() override
		{
			Framework::LoadingSpinner(
				"##spinner", 30,
				sf::Color::Color(230, 230, 230),
				sf::Color::Color(150, 150, 150),
				8, 4.f);

			ImGui::SetWindowSize(
				(sf::Vector2f) ImGui::GetWindowContentRegionMin() + sf::Vector2f{ 20, 20 }
			);
		}

		bool makeWindow() override
		{
			ImGuiIO& io = ImGui::GetIO();
			ImGui::SetNextWindowPos(
				ImVec2(io.DisplaySize.x * 0.99f, io.DisplaySize.y * 0.99f),
				0,
				ImVec2{ 1.f, 1.f });

			ImGui::PushStyleColor(ImGuiCol_ModalWindowDimBg, dimColor);
			return IndependentModalWindow::makeWindow();
		}

		void postWindow() override
		{
			ImGui::PopStyleColor();
		}
	};
}