#pragma once

#include "ImGuiUtil.h"

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

	private:
		bool active = true;
	};
}
