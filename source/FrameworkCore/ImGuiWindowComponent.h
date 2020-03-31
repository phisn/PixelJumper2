#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

#include <string>

#include <Logger/Logger.h>

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
			return ImGui::Begin(
				title.c_str(),
				useActive ? &active : NULL,
				windowFlags);
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

	public:
		constexpr static ImGuiWindowFlags WindowFlagsDefault =
			ImGuiWindowFlags_NoSavedSettings;

		constexpr static ImGuiWindowFlags WindowFlagsStatic =
			WindowFlagsDefault |
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_AlwaysAutoResize;

		constexpr static ImGuiWindowFlags WindowFlagsHidden =
			WindowFlagsStatic |
			ImGuiWindowFlags_NoBackground;

		constexpr static ImGuiWindowFlags WindowFlagsOverlay =
			WindowFlagsHidden |
			ImGuiWindowFlags_NoMouseInputs |
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoScrollWithMouse;

	private:
		bool active = true;
	};
}
