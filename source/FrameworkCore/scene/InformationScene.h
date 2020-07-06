#pragma once

#include "FrameworkCore/FrameworkCore.h"
#include "FrameworkCore/imgui/InformationModalWindow.h"

namespace Scene
{
	// informative scene does use popscene and not popchildscene
	// therefore when the information scene is pushed as a child it will close
	// the pushing scene
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
				// see above
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

namespace Framework
{
	inline void PushErrorScene(std::string message)
	{
		Core::PushChildScene(new ::Scene::InformationScene{ message, "OK" });
	}
}
