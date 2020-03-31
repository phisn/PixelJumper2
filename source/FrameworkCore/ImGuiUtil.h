#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

#include <algorithm>
#include <cctype>
#include <cmath>

namespace Framework
{
	static void ImGuiApplyColorStyle()
	{
		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 0.95f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.12f, 0.12f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.94f);
		colors[ImGuiCol_Border] = ImVec4(0.53f, 0.53f, 0.53f, 0.46f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.85f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.22f, 0.22f, 0.22f, 0.40f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.16f, 0.16f, 0.16f, 0.53f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.48f, 0.48f, 0.48f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.79f, 0.79f, 0.79f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.48f, 0.47f, 0.47f, 0.91f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.55f, 0.55f, 0.62f);
		colors[ImGuiCol_Button] = ImVec4(0.50f, 0.50f, 0.50f, 0.63f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.67f, 0.67f, 0.68f, 0.63f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.26f, 0.26f, 0.26f, 0.63f);
		colors[ImGuiCol_Header] = ImVec4(0.54f, 0.54f, 0.54f, 0.58f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.64f, 0.65f, 0.65f, 0.80f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.25f, 0.25f, 0.25f, 0.80f);
		colors[ImGuiCol_Separator] = ImVec4(0.58f, 0.58f, 0.58f, 0.50f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.81f, 0.81f, 0.81f, 0.64f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.81f, 0.81f, 0.81f, 0.64f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.87f, 0.87f, 0.87f, 0.53f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.87f, 0.87f, 0.87f, 0.74f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.87f, 0.87f, 0.87f, 0.74f);
		colors[ImGuiCol_Tab] = ImVec4(0.01f, 0.01f, 0.01f, 0.86f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
		colors[ImGuiCol_TabActive] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.02f, 0.02f, 0.02f, 1.00f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
		colors[ImGuiCol_DockingPreview] = ImVec4(0.38f, 0.48f, 0.60f, 1.00f);
		colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.68f, 0.68f, 0.68f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.77f, 0.33f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.87f, 0.55f, 0.08f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.47f, 0.60f, 0.76f, 0.47f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(0.58f, 0.58f, 0.58f, 0.90f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.75f);
	}

	inline bool InputTextKey(
		const char* label,
		const char* hint,
		char* buf,
		int buf_size,
		const ImVec2& size_arg = ImVec2{ 0, 0 })
	{
		return ImGui::InputTextEx(label, hint, buf, buf_size, size_arg,
			ImGuiInputTextFlags_CallbackCharFilter |
			ImGuiInputTextFlags_CallbackAlways,
			[](ImGuiInputTextCallbackData* const data) -> int
			{					
				switch (data->EventFlag)
				{
				case ImGuiInputTextFlags_CallbackCharFilter:
					if (data->EventChar >= 'A' && data->EventChar <= 'Z' ||
						data->EventChar >= 'a' && data->EventChar <= 'z')
					{
						data->EventChar = toupper(data->EventChar);

						return 0;
					}

					return 1;

				case ImGuiInputTextFlags_CallbackAlways:
					static int previous_text_len = data->CursorPos;

					if (previous_text_len != data->BufTextLen)
					{
						if (data->BufTextLen > 5 && data->Buf[5] != '-' ||
							data->BufTextLen > 11 && data->Buf[11] != '-')
						{
							int tempSize = data->BufTextLen;
							char temp[18];
							memcpy(temp, data->Buf, data->BufTextLen);

							data->DeleteChars(0, data->BufTextLen);

							int tempIndex = 0, bufIndex = 0;
							while (tempIndex < tempSize)
							{
								char insertBuffer[2] = { };

								if ((bufIndex + 1) % 6 == 0)
								{
									if (temp[tempIndex] == '-')
									{
										tempIndex++;
									}

									insertBuffer[0] = '-';
								}
								else
								{
									insertBuffer[0] = temp[tempIndex++];
								}

								data->InsertChars(bufIndex++, insertBuffer);
							}
						}

						if (previous_text_len > data->BufTextLen&&
							data->BufTextLen && (data->BufTextLen) % 6 == 0)
						{
							data->DeleteChars(data->BufTextLen - 1, 1);
						}

						previous_text_len = data->BufTextLen;
					}


					break;
				}

			}, NULL);
	}

	inline void LoadingSpinner(const char* label, const float indicator_radius,
		const ImVec4& main_color, const ImVec4& backdrop_color,
		const int circle_count, const float speed) {
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems) {
			return;
		}

		ImGuiContext& g = *GImGui;
		const ImGuiID id = window->GetID(label);

		const ImVec2 pos = window->DC.CursorPos;
		const float circle_radius = indicator_radius / 10.0f;
		const ImRect bb(pos, ImVec2(pos.x + indicator_radius * 2.2f,
			pos.y + indicator_radius * 2.2f));
		ImGui::ItemSize(bb, ImGui::GetStyle().FramePadding.y);
		if (!ImGui::ItemAdd(bb, id)) {
			return;
		}
		const float t = g.Time;
		const auto degree_offset = 2.0f * IM_PI / circle_count;
		for (int i = 0; i < circle_count; ++i) {
			const auto x = indicator_radius * std::sin(degree_offset * i) + indicator_radius * 0.1;
			const auto y = indicator_radius * std::cos(degree_offset * i) - indicator_radius * 0.1;
			const auto growth = std::max(0.0f, std::sin(t * speed - i * degree_offset));
			ImVec4 color;
			color.x = main_color.x * growth + backdrop_color.x * (1.0f - growth);
			color.y = main_color.y * growth + backdrop_color.y * (1.0f - growth);
			color.z = main_color.z * growth + backdrop_color.z * (1.0f - growth);
			color.w = 1.0f;
			window->DrawList->AddCircleFilled(ImVec2(pos.x + indicator_radius + x,
				pos.y + indicator_radius - y),
				circle_radius + growth * circle_radius,
				ImGui::GetColorU32(color));
		}
	}
	
	inline void ImGuiSetRelativeNextWindowPos(
		ImVec2 position,
		ImGuiCond condition = 0,
		ImVec2 pivot = ImVec2(0.5f, 0.5f))
	{
		ImGuiIO& io = ImGui::GetIO();
		ImGui::SetNextWindowPos(
			ImVec2(io.DisplaySize.x * position.x, io.DisplaySize.y * position.y),
			condition,
			pivot);
	}
}
