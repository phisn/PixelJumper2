#pragma once

#include "Client/game/ControllablePlayer.h"

#include "FrameworkCore/FrameworkCore.h"
#include "FrameworkCore/ViewChain.h"
#include "GameCore/ClassicSimulation.h"
#include "Logger/Logger.h"
#include "ResourceCore/ResourceInterface.h"

#include "GameCore/component/SolidCollisionComponent.h"
#include "GameCore/component/StaticVisibleComponent.h"
#include "GameCore/component/TileComponent.h"

#include <imgui/imgui.h>

namespace Scene
{
	class TestGameScene
		:
		public Framework::Scene
	{
	public:
		TestGameScene()
			:
			simulation(resource_container),
			viewChain(1),
			player(
				Device::Input::Player::P1,
				Game::PlayerInformation{ },
				&viewChain.getView(0))
		{
		}

		bool onCreate() override
		{
			Resource::World* const world = new Resource::World;

			world->content.author = 0xff00000bb00000ff;
			world->content.defaultPlayerProperties.inputForce = 1.f;
			world->content.defaultPlayerProperties.inputReduce = 2.f;
			world->content.defaultPlayerProperties.mass = 1.f;
			world->content.defaultPlayerProperties.movement = { 0.f, 0.f };
			world->content.defaultPlayerProperties.position = { 0.f, 0.f };
			world->content.defaultPlayerProperties.viewFollow = true;
			world->content.defaultPlayerProperties.viewRotation = 0;
			world->content.defaultPlayerProperties.viewWindow = { 0.f, 0.f, 100.f, 100.f };

			world->content.defaultProperties.airResistance = 1.f;
			world->content.defaultProperties.gravity = { 0.f, 1.f };
			world->content.id = 0xdd0bb0dd;

			world->content.width = 10;
			world->content.height = 6;

			Resource::TileComponent* c1 = new Resource::TileComponent;
			
			c1->content.position = { 0, 5 };
			c1->content.size = { 10, 1 };

			world->components.emplace_back(Resource::ComponentID::Tile, c1);
			Resource::SolidCollisionComponent* c2 = new Resource::SolidCollisionComponent;

			c2->content.density = Shared::ValueDefinition::tile_density;
			c2->content.friction = Shared::ValueDefinition::tile_friction;
			c2->content.inputForceAddition = Shared::ValueDefinition::tile_input;

			world->components.emplace_back(Resource::ComponentID::SolidCollision, c2);
			Resource::StaticVisibleComponent* c3 = new Resource::StaticVisibleComponent;

			c3->content.color = sf::Color::Color(200, 200, 200);

			world->components.emplace_back(Resource::ComponentID::StaticVisible, c3);

			world->entities.emplace_back();

			world->entities.back().componentIndex.push_back(0);
			world->entities.back().componentIndex.push_back(1);
			world->entities.back().componentIndex.push_back(2);

			if (!Resource::Interface::SaveResource(world, Resource::ResourceType::World, L"world"))
			{
				delete world;
				return false;
			}

			if (!Resource::Interface::LoadResource(
					world,
					Resource::ResourceType::World,
					L"world"))
			{
				delete world;
				return false;
			}

			resource_container[world->content.id] = world;

			simulation.setPlayer(&player);

			if (Game::ClassicSimulation::WorldFailure failure = simulation.runWorld(resource_container.begin()->first); 
				Game::ClassicSimulation::WorldFailure::Success != failure)
			{
				Log::Error(L"failed to run world",
					(int) failure, L"reason");

				return false;
			}

			return true;
		}

		void initialize() override
		{
		}

		void onEvent(const sf::Event event) override
		{
			if (event.type == sf::Event::Resized)
			{
				viewChain.remakeViewViewports();
			}
		}

		void onLogic(const sf::Time time) override
		{
			player.onLogic(time);

			static sf::Uint64 counter;
			counter += time.asMicroseconds();

			while (counter > Game::LogicTimeStep)
			{
				simulation.processLogic();
				counter -= Game::LogicTimeStep;
			}

			if (my_tool_active)
			{
				ImGui::Begin("My First Tool", &my_tool_active, ImGuiWindowFlags_MenuBar);
				if (ImGui::BeginMenuBar())
				{
					if (ImGui::BeginMenu("File"))
					{
						if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
						if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Do stuff */ }
						if (ImGui::MenuItem("Close", "Ctrl+W")) { my_tool_active = false; }
						ImGui::EndMenu();
					}
					ImGui::EndMenuBar();
				}

				// Edit a color (stored as ~4 floats)
				ImGui::ColorEdit4("Color", colors);

				// Plot some values
				static float my_values[128];
				for (int i = 1; i < IM_ARRAYSIZE(my_values); ++i)
				{
					my_values[i - 1] = my_values[i];
				}

				my_values[IM_ARRAYSIZE(my_values) - 1] = 1000.f / (time.asMicroseconds() / 1000.f);

				ImGui::PlotLines("Frame Times", my_values, IM_ARRAYSIZE(my_values));
				static char buffer[16];
				ImGui::InputText("text", buffer, 16);
				// Display contents in a scrolling region
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "Important Stuff");
				ImGui::BeginChild("Scrolling");
				for (int n = 0; n < 50; n++)
					ImGui::Text("%04d: Some text", n);
				ImGui::EndChild();
				ImGui::End();
			}
		}

		void onDraw(sf::RenderTarget* const target) override
		{
			player.enableView(target);
			simulation.draw(target);
			player.onDraw(target);
		}

		void onShow() override
		{
		}

		void onHide() override
		{
		}

	private:
		float colors[4];
		bool my_tool_active = true;

		Framework::ViewChain viewChain;

		Game::VisualClassicSimulation simulation;
		Game::ControllablePlayer player;

		Game::WorldResourceContainer resource_container;
	};
}
