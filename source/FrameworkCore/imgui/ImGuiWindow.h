#pragma once

#include "ImGuiUtil.h"

#include "Common/RandomModule.h"
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
			for (int i = 0; i < 8; ++i)
			{
				tag.push_back('A' + Module::Random::MakeRandom<int32_t>() % ('Z' - 'A'));
			}
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
		ImGuiWindowFlags windowFlags;

		bool windowFocused;

		sf::Vector2f windowSize;
		sf::Vector2f windowPosition;

		void setTitle(std::string title)
		{
			this->title = title + "###" + tag;
		}

	private:
		std::string title;
		std::string tag;

		bool active = true;
	};
}
