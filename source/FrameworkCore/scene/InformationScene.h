#pragma once

#include "FrameworkCore/FrameworkCore.h"
#include "FrameworkCore/imgui/InformationModalWindow.h"

namespace Scene
{
	class InformationScene
		:
		public Framework::Scene
	{
	public:
		InformationScene(std::string text)
			:
			InformationScene(text, "Close")
		{
		}

		InformationScene(std::string text, std::string button)
		{
			window.open(text, button);
		}

		void onEvent(const sf::Event event) override
		{
		}

		void onLogic(const sf::Time time) override
		{
			ImGui::PushStyleColor(ImGuiCol_ModalWindowDimBg, sf::Color::Black);

			if (!window.process())
			{
				Framework::Core::PopScene();
			}

			ImGui::PopStyleColor();
		}

		void onDraw(sf::RenderTarget* const target) override
		{
		}

	private:
		Framework::InformationModalWindow window;
	};
}
