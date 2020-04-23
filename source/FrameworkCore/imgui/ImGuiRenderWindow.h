#pragma once

#include "ImGuiWindow.h"

#include <imgui-sfml/imgui-SFML.h>
#include <SFML/Graphics.hpp>

namespace Framework
{
	class ImGuiRenderWindow
		:
		public ImGuiWindowComponent
	{
	public:
		using ImGuiWindowComponent::ImGuiWindowComponent;

		void process()
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
			if (begin())
			{
				if (renderTextureFailure)
				{
					ImGui::Text("Failed to create rendertexture");

					if (ImGui::Button("Retry"))
						renderTextureFailure = false;
				}
				else
				{
					renderPosition = windowPosition + (sf::Vector2f) ImGui::GetCursorPos();
					renderSize = (sf::Vector2f) ImGui::GetContentRegionMax() - (sf::Vector2f) ImGui::GetCursorPos();

					if (sf::Vector2u(renderSize) != renderTexture.getSize())
					{
						Log::Information(L"recreate");
						if (!renderTexture.create(renderSize.x, renderSize.y))
						{
							renderTextureFailure = true;
						}
					}

					onDraw(&renderTexture);
					renderTexture.display();
					_process();

					ImGui::Image(
						renderTexture.getTexture(),
						renderSize,
						tintColor, borderColor);
				}
			}

			ImGui::PopStyleVar();
			end();
		}

		bool hasRenderTextureFailure() const
		{
			return renderTextureFailure;
		}

		virtual void _process() = 0;

	protected:
		sf::Color tintColor = sf::Color::White;
		sf::Color borderColor = sf::Color::Transparent;

		virtual void onDraw(sf::RenderTarget* target) = 0;

		using ImGuiWindowComponent::begin;
		using ImGuiWindowComponent::end;

		sf::Vector2f renderSize;
		sf::Vector2f renderPosition;

		sf::RenderTexture renderTexture;

		sf::Event convertWindowEvent(sf::Event event)
		{
			switch (event.type)
			{
			case sf::Event::EventType::MouseButtonPressed:
				event.mouseButton.x -= renderPosition.x;
				event.mouseButton.y -= renderPosition.y;

				break;
			case sf::Event::EventType::MouseButtonReleased:
				event.mouseButton.x -= renderPosition.x;
				event.mouseButton.y -= renderPosition.y;

				break;
			case sf::Event::EventType::MouseMoved:
				event.mouseMove.x -= renderPosition.x;
				event.mouseMove.y -= renderPosition.y;

				break;
			case sf::Event::EventType::MouseWheelScrolled:
				event.mouseWheel.x -= renderPosition.x;
				event.mouseWheel.y -= renderPosition.y;

				break;
			}

			return event;
		}

	private:
		bool renderTextureFailure = false;
	};
}
