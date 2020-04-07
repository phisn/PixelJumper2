#pragma once

#include "ImGuiUtil.h"

#include <string>

namespace Framework
{
	class ModalWindow
	{
	public:
		ModalWindow()
			:
			windowFlags(WindowFlagsDefault)
		{
		}

		ModalWindow(ImGuiWindowFlags windowFlags)
			:
			windowFlags(windowFlags)
		{
		}

		bool isActive() const
		{
			return active;
		}

		const std::string& getTitle() const
		{
			return title;
		}

		// might be exposed
	protected:
		virtual bool begin()
		{
			if (active && !opened)
			{
				ImGui::OpenPopup(title.c_str());
				opened = true;
			}

			preWindow();

			bool result = ImGui::BeginPopupModal(
				title.c_str(),
				useActive ? &active : NULL,
				windowFlags);

			if (!result)
			{
				postWindow();
			}

			if (!active && opened)
			{
				opened = false;

				if (result)
				{
					ImGui::CloseCurrentPopup();
					ImGui::EndPopup();
				}

				return false;
			}

			return result;
		}

		virtual void end()
		{
			ImGui::End();
			postWindow();
		}

		virtual void open()
		{
			active = true;
		}

		virtual void close()
		{
			active = false;
		}

		// must only be used inside begin / end
		// for this window
		virtual void closeWindow()
		{
			close();
			ImGui::CloseCurrentPopup();
			opened = false;
		}

		// internal use only
	protected:
		std::string title;
		bool useActive;
		ImGuiWindowFlags windowFlags;

		virtual void preWindow()
		{
		}

		virtual void postWindow()
		{
		}

	private:
		bool active;
		bool opened;
	};

	class IndependentModalWindow
		:
		public ModalWindow
	{
	public:
		using ModalWindow::ModalWindow;

		bool process()
		{
			if (begin())
			{
				onContent();
				end();

				return true;
			}

			return false;
		}

	protected:
		virtual void onContent() = 0;
	};
}
