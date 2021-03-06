#pragma once

#include "ImGuiUtil.h"

#include "Common/RandomModule.h"

#include <string>

namespace Framework
{
	class PurePopupWindow
	{
	public:
		PurePopupWindow()
			:
			PurePopupWindow(WindowFlagsDefault)
		{
		}

		PurePopupWindow(ImGuiWindowFlags windowFlags)
			:
			windowFlags(windowFlags)
		{
			for (int i = 0; i < 8; ++i)
			{
				tag.push_back('A' + Module::Random::MakeRandom<int32_t>() % ('A' - 'Z'));
			}
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
			bool wasActive = active;

			if (active && !opened)
			{
				ImGui::OpenPopup(title.c_str());
			}

			opened = makeWindow();

			if (!opened)
			{
				postWindow();
			}

			if (active != opened)
			{
				if (opened)
				{
					ImGui::CloseCurrentPopup();
					ImGui::EndPopup();

					opened = false;
				}
				else
				{
					active = false;
				}
			}

			return opened;
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
		ImGuiWindowFlags windowFlags;

		virtual bool makeWindow() = 0;
		virtual void postWindow()
		{
		}

		bool active = false;
		bool opened = false;

		void setTitle(std::string title)
		{
			this->title = title + "###" + tag;
		}

	private:
		std::string title;
		std::string tag;
	};

	class PopupWindow
		:
		public PurePopupWindow
	{
	public:
		using PurePopupWindow::PurePopupWindow;

	protected:
		virtual bool makeWindow() override
		{
			return ImGui::BeginPopup(
				getTitle().c_str(),
				windowFlags);
		}
	};

	class ModalWindow
		:
		public PurePopupWindow
	{
	public:
		using PurePopupWindow::PurePopupWindow;
		
	protected:
		bool useActive;

		virtual bool makeWindow() override
		{
			return ImGui::BeginPopupModal(
				getTitle().c_str(),
				useActive ? &active : NULL,
				windowFlags);
		}
	};

	template <typename PopupWindow>
	class IndependentPurePopupWindow
		:
		public PopupWindow
	{
	public:
		using PopupWindow::PopupWindow;

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

	typedef IndependentPurePopupWindow<PopupWindow> IndependentPopupWindow;
	typedef IndependentPurePopupWindow<ModalWindow> IndependentModalWindow;
}
