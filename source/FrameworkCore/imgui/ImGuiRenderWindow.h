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
					sf::Vector2u size = ImGui::GetWindowSize();

					if (size != renderTexture.getSize())
					{
						if (!renderTexture.create(size.x, size.y))
						{
							renderTextureFailure = true;
						}
					}

					onDraw(&renderTexture);
					ImGui::Image(
						renderTexture.getTexture(),
						(ImVec2)size,
						tintColor, borderColor);
				}
			}

			end();
		}

		bool hasRenderTextureFailure() const
		{
			return renderTextureFailure;
		}

	protected:
		sf::Color tintColor = sf::Color::White;
		sf::Color borderColor = sf::Color::Transparent;

		virtual void onDraw(sf::RenderTarget* target) = 0;

		using ImGuiWindowComponent::begin;
		using ImGuiWindowComponent::end;

	private:
		bool renderTextureFailure = false;
		sf::RenderTexture renderTexture;
	};
}
