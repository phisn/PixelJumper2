#pragma once

#include "FrameworkCore/FrameworkCore.h"
#include "FrameworkCore/scene/InformationScene.h"
#include "FrameworkCore/scene/QuestionScene.h"

#include <optional>

namespace Editor
{
	typedef std::optional<std::function<bool()>> EditorRepairData;

	class EditorFailureScene
		:
		public Framework::Scene
	{
	public:
		EditorFailureScene(std::string reason)
			:
			EditorFailureScene(reason, EditorRepairData{ })
		{
		}

		EditorFailureScene(std::string reason, EditorRepairData repair)
			:
			reason(reason),
			repair(repair)
		{
		}

		void onEvent(const sf::Event event) override
		{
		}

		void onLogic(const sf::Time time) override
		{
			ImGui::Begin("Internal Editor Failure");
			ImGui::Text("Reason: %s", reason.c_str());

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			if (ImGui::Button("Close"))
			{
				Framework::Core::PopScene();
			}

			ImGui::SameLine();

			if (repair && ImGui::Button("Repair"))
			{
				Framework::Core::PushChildScene(
					new ::Scene::QuestionScene(
						[this](bool result)
						{
							if (result)
							{
								if (repair.value()())
								{
									Framework::Core::PushChildScene(
										new ::Scene::InformationScene(
											"Succeeded to repair data\nData might still be broken",
											"OK"),
										true);
								}
								else
								{
									Framework::Core::PushScene(
										new ::Scene::InformationScene(
											"Failed to repair data",
											"OK"));
								}
							}
							else
							{
								Framework::Core::PopChildScene();
							}
						},
						"Do you really want to repair?\n(Some data will be lost)"),
					true);
			}

			if (ImGui::Button("Reset"))
			{
				Framework::Core::PushChildScene(
					new ::Scene::QuestionScene(
						[this](bool result)
						{
							if (result)
							{
								Framework::Core::PushChildScene(
									new ::Scene::QuestionScene(
										[this](bool result)
										{
											if (result)
											{
												// remove all data
											}
											else
											{
												Framework::Core::PopChildScene();
											}

										}, "Do you REAAALY want to reset all data?\n(This action can not be undone!)"),
									false);
							}
							else
							{
								Framework::Core::PopChildScene();
							}
						},
						"Do you really want to reset all data?\n(All data will be lost)"),
					true);
			}

			ImGui::End();
		}

		void onDraw(sf::RenderTarget* const target) override
		{
		}

	private:
		EditorRepairData repair;
		std::string reason;
	};
}
