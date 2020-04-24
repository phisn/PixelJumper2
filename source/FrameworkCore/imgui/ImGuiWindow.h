#pragma once

#include "ImGuiUtil.h"

#include "Logger/Logger.h"

#include <SFML/Graphics.hpp>
#include <string>

namespace Framework
{
	class ImGuiWindowComponent
	{
	public:
		ImGuiWindowComponent()
			:
			windowFlags(WindowFlagsDefault)
		{
		}

		ImGuiWindowComponent(ImGuiWindowFlags windowFlags)
			:
			windowFlags(windowFlags)
		{
		}

		virtual bool begin()
		{
			assert(title.size() != 0);

			bool result = ImGui::Begin(
				title.c_str(),
				useActive ? &active : NULL,
				windowFlags);

			if (result)
			{
				windowFocused = ImGui::IsWindowFocused();
				windowSize = ImGui::GetWindowSize();
				windowPosition = ImGui::GetWindowPos();
			}

			return result;
		}

		virtual void end()
		{
			ImGui::End();
		}

		bool isActive() const
		{
			return active;
		}

	protected:
		bool useActive = false;
		std::string title;
		ImGuiWindowFlags windowFlags;

		bool windowFocused;

		sf::Vector2f windowSize;
		sf::Vector2f windowPosition;

	private:
		bool active = true;
	};
}
