/*
#include <iostream>
#include <SFML/Graphics.hpp>

int main()
{
	sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
	sf::CircleShape shape(10.f);
	shape.setFillColor(sf::Color::Green);

	sf::View view;
	view.setSize(100, 100);
	view.setCenter(view.getSize() / 2.f);

	sf::RenderTexture rt;
	rt.create(1000, 1000);

	sf::Sprite sprite;
	sprite.setTexture(rt.getTexture());

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::EventType::MouseButtonPressed)
			{
				sf::Vector2f coords = rt.mapPixelToCoords({ event.mouseButton.x, event.mouseButton.y }, view);
				std::cout << coords.x << " : " << coords.y << std::endl;
			}
		}

		window.clear();
		rt.setView(view);
		rt.draw(shape);
		window.draw(sprite);
		window.display();
	}

	return 0;
}*/


#include "EditorCore/EditorScene.h"
#include "FrameworkCore/ScreenDevice.h"

#include "imgui-sfml/imgui-SFML.h"

int main()
{
	Log::Output::Add(Log::Output::CONSOLE_OUT, Log::Level::Information);

	if (!Device::Screen::Initialize())
	{
		return 2;
	}

	if (!Framework::Core::Initialize())
	{
		return 3;
	}

	Framework::Core::PushScene<Scene::EditorScene>();

	ImGui::SFML::Init(*Device::Screen::GetWindow());

	ImGui::GetStyle().WindowRounding = 0;
	ImGui::StyleColorsLight();

	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	sf::Clock clock;
	sf::Event event;

	while (true)
	{
		while (Device::Screen::GetWindow()->pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);
			Framework::Core::ProcessEvent(event);
		}

		const sf::Time delta = clock.restart();

		ImGui::SFML::Update(
			*Device::Screen::GetWindow(),
			delta);
		Framework::Core::ProcessLogic(delta);
		
		if (!Framework::Core::IsRunning())
		{
			break;
		}

		Device::Screen::GetWindow()->clear();
		Framework::Core::ProcessDraw(Device::Screen::GetWindow());
		ImGui::SFML::Render(*Device::Screen::GetWindow());
		Device::Screen::GetWindow()->display();
	}

	Device::Screen::Uninitialize();
	ImGui::SFML::Shutdown();

	return 0;
}