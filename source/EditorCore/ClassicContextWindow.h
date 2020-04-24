#pragma once

#include "FrameworkCore/imgui/ImGuiGridWindow.h"

namespace Editor
{
	class ClassicContextPopup
	{
	public:
		void open(int x, int y)
		{
			isOpen = true;
			isVisible = false;
		}

		void process()
		{
			if (isOpen && !isVisible)
			{
				ImGui::OpenPopup("");
			}
		}

	private:
		bool isOpen, isVisible;
	};

	class ClassicContextWindow
		:
		public Framework::ImGuiGridWindow
	{
	public:
		ClassicContextWindow()
		{
			title = "ClassicContext";
		}

		void onEvent(sf::Event event) override
		{
			ImGuiGridWindow::onEvent(event);

			switch (event.type)
			{
			case sf::Event::EventType::MouseButtonPressed:
				break;
			}
		}

	private:
		void onMovementClick(int x, int y) override
		{
		}

		void onDraw(sf::RenderTarget* target) override
		{
			ImGuiGridWindow::onDraw(target);
		}

		void processContent() override
		{
			ImGuiGridWindow::processContent();

			if (ImGui::BeginPopup("context_root_popup"))
			{
				ImGui::Button("hello world");

				ImGui::EndPopup();
			}
		}
	};
}

