#include "imgui/imgui.h"
#include "imgui-sfml/imgui-SFML.h"
#include "imgui/imgui_internal.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/CircleShape.hpp>

int main()
{
    sf::RenderWindow window(sf::VideoMode(640, 480), "ImGui + SFML = <3");
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);
    ImGui::GetStyle().WindowRounding = 0;
    ImGui::StyleColorsLight();

    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGuiStyleVar_WindowRounding;
    sf::Clock deltaClock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);

            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        ImGui::SFML::Update(window, deltaClock.restart());
        ImGui::ShowMetricsWindow();

        ImGui::SetNextWindowPos(ImGui::GetIO().DisplaySize, ImGuiCond_Appearing, ImVec2{ 1.f, 1.0f });
        ImGui::Begin("Test2", NULL);
        
        if (ImGui::Button("test2"))
        {
            ImGui::OpenPopup("popup");
        }

        ImGui::End();


        if (ImGui::BeginPopup("popup"))
        {
            ImGui::Text("test");

            ImGui::EndPopup();
        }
        /*
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Appearing);
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, ImGuiCond_Appearing);
        ImGui::Begin("Test", NULL, ImGuiWindowFlags_MenuBar |
            ImGuiWindowFlags_NoDocking |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus);
        ImGui::Button("test");

        ImGui::DockSpace(123, ImVec2(0, 0), ImGuiDockNodeFlags_PassthruCentralNode);
        ImGui::End();

        ImGui::Begin("Test2", NULL);
        ImGui::Button("test2");
        ImGui::End();

        ImGui::Begin("Test3", NULL);
        ImGui::Button("test3");
        ImGui::End();
        */
        window.clear();
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
}