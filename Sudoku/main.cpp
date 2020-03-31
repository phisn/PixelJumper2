#include "generator.h"
#include "resource.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui-sfml/imgui-SFML.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include <Windows.h>
#include <WinUser.h>

int WINAPI wWinMain(
    HINSTANCE hInstance, 
    HINSTANCE hPrevInstance, 
    PWSTR pCmdLine, 
    int nCmdShow)
{
    sf::RenderWindow window(sf::VideoMode(640, 480), "");
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);
    ImGui::GetIO().IniFilename = NULL;
    ImGui::StyleColorsLight();

    HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    SendMessage(window.getSystemHandle(), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
    SendMessage(window.getSystemHandle(), WM_SETICON, ICON_BIG, (LPARAM)hIcon);

    srand(time(NULL));

    Sudoku sudoku;
    sudoku.init();
    sudoku.createSeed();
    sudoku.genPuzzle();

    int content[9][9] = { };

    memcpy(content, sudoku.grid, sizeof(sudoku.grid));

    int selected_x, selected_y;

    bool invalidLine_vertical[9] = { };
    bool invalidLine_horizontal[9] = { };
    bool invalidLine_3x3[9] = { };

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

        ImGuiIO& io = ImGui::GetIO();
        ImGui::SetNextWindowPos(
            ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f),
            0,
            ImVec2(0.5f, 0.5f));

        ImGui::Begin("sudoku", NULL,
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_AlwaysAutoResize | 
            ImGuiWindowFlags_HorizontalScrollbar |
            ImGuiWindowFlags_NoSavedSettings);
        ImGui::SetWindowFontScale(2.f);

        bool allFilled = true, allValid = true;
        bool content_changed = false;
        for (int c_x = 0; c_x < 3; ++c_x)
        {
            int c_y = 0;

            while (true)
            {
                ImGui::BeginGroup();

                for (int x = 0; x < 3; ++x)
                {
                    int y = 0;
                    while (true)
                    {
                        char button[] = " ##  ";
                        button[3] = '0' + c_x * 3 + x;
                        button[4] = '0' + c_y * 3 + y;

                        if (content[c_x * 3 + x][c_y * 3 + y])
                            button[0] = '0' + content[c_x * 3 + x][c_y * 3 + y];

                        sf::Color color;
                        if (invalidLine_vertical[c_x * 3 + x] ||
                            invalidLine_horizontal[c_y * 3 + y] ||
                            invalidLine_3x3[c_x * 3 + c_y])
                        {
                            if (sudoku.grid[c_x * 3 + x][c_y * 3 + y])
                            {
                                color = sf::Color::Color(170, 170, 170);
                            }
                            else
                            {
                                color = sf::Color::Color(200, 100, 100);
                            }
                        }
                        else
                        {
                            if (sudoku.grid[c_x * 3 + x][c_y * 3 + y])
                            {
                                color = sf::Color::Color(160, 160, 160);
                            }
                            else
                            {
                                color = sf::Color::Color(200, 200, 200);
                            }
                        }

                        ImGui::PushStyleColor(
                            ImGuiCol_::ImGuiCol_Button,
                            color);

                        if (!sudoku.grid[c_x * 3 + x][c_y * 3 + y])
                        {
                            if (ImGui::Button(button, ImVec2{ 40, 40 }))
                            {
                                selected_x = c_x * 3 + x;
                                selected_y = c_y * 3 + y;

                                content_changed = true;

                                ImGui::OpenPopup("selected");
                            }
                        }
                        else
                        {
                            ImGui::ButtonEx(button, ImVec2{ 40, 40 }, ImGuiButtonFlags_::ImGuiButtonFlags_Disabled);
                        }

                        if (content_changed == false && ImGui::IsItemHovered() && !sudoku.grid[c_x * 3 + x][c_y * 3 + y])
                        {
                            if (ImGui::GetIO().MouseDown[1])
                            {
                                selected_x = c_x * 3 + x;
                                selected_y = c_y * 3 + y;

                                content[selected_x][selected_y] = 0;

                                content_changed = true;
                            }
                            else
                            {
                                if (content_changed == false)
                                    for (int i = 0; i < 10; ++i)
                                        if (ImGui::GetIO().KeysDown[sf::Keyboard::Num0 + i])
                                        {
                                            content_changed = true;

                                            selected_x = c_x * 3 + x;
                                            selected_y = c_y * 3 + y;

                                            content[selected_x][selected_y] = i;

                                            break;
                                        }

                                if (content_changed == false)
                                    for (int i = 0; i < 10; ++i)
                                        if (ImGui::GetIO().KeysDown[sf::Keyboard::Numpad0 + i])
                                        {
                                            content_changed = true;

                                            selected_x = c_x * 3 + x;
                                            selected_y = c_y * 3 + y;

                                            content[selected_x][selected_y] = i;

                                            break;
                                        }
                            }
                        }

                        if (content[c_x * 3 + x][c_y * 3 + y] == 0)
                        {
                            allFilled = false;
                        }

                        ImGui::PopStyleColor();

                        if (++y >= 3)
                        {
                            break;
                        }

                        ImGui::SameLine(0.0f, 4.f);
                    }

                    ImGui::NextColumn();
                }

                ImGui::EndGroup();

                if (++c_y >= 3)
                {
                    break;
                }

                ImGui::SameLine(0.0f, 3.f);
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
                ImGui::SameLine(0.0f, 4.f);
            }

            if (c_x != 2)
            {
                ImGui::Separator();
            }

            ImGui::NextColumn();
        }

        if (ImGui::BeginPopup("selected"))
        {
            for (int i = 1; i < 10; ++i)
            {
                char button[] = " ## popup";
                button[0] = '0' + i;

                if (ImGui::Button(button, ImVec2(30, 30)) || ImGui::GetIO().KeysDown[sf::Keyboard::Num0 + i])
                {
                    content[selected_x][selected_y] = i;

                    content_changed = true;

                    ImGui::CloseCurrentPopup();

                    break;
                }

                if (i % 3 == 0)
                {
                    ImGui::NextColumn();
                }
                else
                {
                    ImGui::SameLine(0.f, 4.f);
                }
            }

            ImGui::EndPopup();
        }

        if (content_changed)
        {
            content_changed = false;

            bool isDouble = false;
            bool buffer[9] = { };

            // vertical
            for (int j = 0; j < 9; ++j)
            {
                int index = content[selected_x][j] - 1;

                if (index == -1)
                {
                    continue;
                }

                if (buffer[index])
                {
                    isDouble = true;
                    break;
                }

                buffer[index] = true;
            }

            invalidLine_vertical[selected_x] = isDouble;

            isDouble = false;
            memset(buffer, 0, sizeof(buffer));

            // horizontal
            for (int j = 0; j < 9; ++j)
            {
                int index = content[j][selected_y] - 1;

                if (index == -1)
                {
                    continue;
                }

                if (buffer[index])
                {
                    isDouble = true;
                    break;
                }

                buffer[index] = true;
            }

            invalidLine_horizontal[selected_y] = isDouble;

            isDouble = false;
            memset(buffer, 0, sizeof(buffer));

            // 3x3
            for (int j = 0; j < 9; ++j)
            {
                int index = content[3 * (selected_x / 3) + (j % 3)][3 * (selected_y / 3) + (j / 3)] - 1;

                if (index == -1)
                {
                    continue;
                }

                if (buffer[index])
                {
                    isDouble = true;
                    break;
                }

                buffer[index] = true;
            }

            invalidLine_3x3[3 * (selected_x / 3) + (selected_y / 3)] = isDouble;
        }

        if (!ImGui::IsPopupOpen(ImGui::GetID("reset")))
        {
            if (ImGui::GetIO().KeysDown[sf::Keyboard::R])
            {
                ImGui::OpenPopup("reset");
            }
        }

        if (ImGui::BeginPopupModal("reset", NULL, 
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoTitleBar))
        {
            if (ImGui::Button("Reset", ImVec2{ 150, 0 }))
            {
                sudoku.init();
                sudoku.createSeed();
                sudoku.genPuzzle();

                memcpy(content, sudoku.grid, sizeof(sudoku.grid));

                memset(invalidLine_vertical, 0, sizeof(invalidLine_vertical));
                memset(invalidLine_horizontal, 0, sizeof(invalidLine_horizontal));
                memset(invalidLine_3x3, 0, sizeof(invalidLine_3x3));

                ImGui::CloseCurrentPopup();
            }
            else
            {
                if (ImGui::Button("Cancel", ImVec2{ 150, 0 }) ||
                    ImGui::GetIO().KeysDown[sf::Keyboard::Escape])
                {
                    ImGui::CloseCurrentPopup();
                }
            }

            ImGui::EndPopup();
        }

        if (allValid && allFilled)
            for (int i = 0; i < 9; ++i)
                if (invalidLine_vertical[i])
                {
                    allValid = false;
                }

        if (allValid && allFilled)
            for (int i = 0; i < 9; ++i)
                if (invalidLine_horizontal[i])
                {
                    allValid = false;
                }

        if (allValid && allFilled)
            for (int i = 0; i < 9; ++i)
                if (invalidLine_3x3[i])
                {
                    allValid = false;
                }

        if (allValid && allFilled)
        {
            ImGui::OpenPopup("win");
        }

        if (ImGui::BeginPopupModal("win", NULL,
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoTitleBar))
        {
            ImGui::Text("Done! (%d)", sudoku.branchDifficultyScore());
            if (ImGui::Button("Reset", ImVec2{ 140, 0 }))
            {
                sudoku.init();
                sudoku.createSeed();
                sudoku.genPuzzle();

                memcpy(content, sudoku.grid, sizeof(sudoku.grid));

                memset(invalidLine_vertical, 0, sizeof(invalidLine_vertical));
                memset(invalidLine_horizontal, 0, sizeof(invalidLine_horizontal));
                memset(invalidLine_3x3, 0, sizeof(invalidLine_3x3));

                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    
        const sf::FloatRect rect =
        { 
            ImGui::GetWindowPos().x,
            ImGui::GetWindowPos().y,
            ImGui::GetWindowSize().x,
            ImGui::GetWindowSize().y
        };

        ImGui::End();

        ImGui::SetNextWindowPos(
            ImVec2{
                rect.left,
                rect.top + rect.height
            });
        ImGui::Begin("info", NULL,
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoScrollWithMouse |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoMouseInputs);

        ImGui::PushStyleColor(ImGuiCol_Text, sf::Color::Color(170, 170, 170));
        ImGui::Text("Reset: R");
        ImGui::PopStyleColor();

        ImGui::End();

        ImGui::GetOverlayDrawList();
        window.clear();
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
} 
