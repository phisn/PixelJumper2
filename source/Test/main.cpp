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

/*
#include "bezier.hpp"

#include <cmath>
#include <vector>

#include <SFML/Graphics.hpp>

// DRAW FUNCTIONS
void drawPoints(sf::RenderWindow& window, const std::vector<sf::Vector2f>& points, const sf::Color& color = sf::Color::Red, const float radius = 10.f)
{
    sf::CircleShape circle(radius);
    circle.setFillColor(color);

    for (auto it : points) {
        circle.setPosition(sf::Vector2f(
            it.x - (radius / 2.f),
            it.y - (radius / 2.f)));
        window.draw(circle);
    }
}

void drawControlPoints(sf::RenderWindow& window, const std::vector<sf::Vector2f>& points, const sf::Color& color = sf::Color::Blue, const float radius = 5.f)
{
    sf::CircleShape circle(radius);
    circle.setFillColor(color);

    for (auto it : points) {
        circle.setPosition(sf::Vector2f(
            it.x - (radius / 2.f),
            it.y - (radius / 2.f)));
        window.draw(circle);
    }
}

void drawAnchorPoints(sf::RenderWindow& window, const std::vector<sf::Vector2f>& points, const sf::Color& color = sf::Color::Yellow, const float radius = 5.f)
{
    sf::CircleShape circle(radius);
    circle.setFillColor(color);

    for (auto it : points) {
        circle.setPosition(sf::Vector2f(
            it.x - (radius / 2.f),
            it.y - (radius / 2.f)));
        window.draw(circle);
    }
}

void drawControlLines(sf::RenderWindow& window, const std::vector<sf::Vector2f>& points, const bool isClosed,
    const sf::Color& color = sf::Color::Yellow, const float thickness = 2.f)
{
    sf::RectangleShape line;
    line.setFillColor(color);

    for (int i = 1; i < (int)points.size() - 1; i += 2) {
        sf::Vector2f p1 = points[i];
        sf::Vector2f p2 = points[i + 1];

        line.setSize(sf::Vector2f(std::sqrt(std::pow(p2.x - p1.x, 2) + std::pow(p2.y - p1.y, 2)), thickness));
        line.setOrigin(line.getSize() / 2.f);
        line.setPosition(sf::Vector2f((p1.x + p2.x) / 2.f, (p1.y + p2.y) / 2.f));
        line.setRotation(std::atan2(p2.y - p1.y, p2.x - p1.x) * 180.f / M_PI);
        window.draw(line);
    }

    if (isClosed && points.size() > 1) {
        sf::Vector2f p1 = points[0];
        sf::Vector2f p2 = points[points.size() - 1];

        line.setFillColor(sf::Color(color.r / 2.f, color.g / 2.f, color.b / 2.f));
        line.setSize(sf::Vector2f(std::sqrt(std::pow(p2.x - p1.x, 2) + std::pow(p2.y - p1.y, 2)), thickness));
        line.setOrigin(line.getSize() / 2.f);
        line.setPosition(sf::Vector2f((p1.x + p2.x) / 2.f, (p1.y + p2.y) / 2.f));
        line.setRotation(std::atan2(p2.y - p1.y, p2.x - p1.x) * 180.f / M_PI);
        window.draw(line);
    }
}

void drawPointLines(sf::RenderWindow& window, const std::vector<sf::Vector2f>& points, const bool isClosed,
    const sf::Color& color = sf::Color::Red, const float thickness = 4.f)
{
    sf::RectangleShape line;
    line.setFillColor(color);

    for (int i = 0; i < (int)points.size() - 1; i++) {
        sf::Vector2f p1 = points[i];
        sf::Vector2f p2 = points[i + 1];

        line.setSize(sf::Vector2f(std::sqrt(std::pow(p2.x - p1.x, 2) + std::pow(p2.y - p1.y, 2)), thickness));
        line.setOrigin(line.getSize() / 2.f);
        line.setPosition(sf::Vector2f((p1.x + p2.x) / 2.f, (p1.y + p2.y) / 2.f));
        line.setRotation(std::atan2(p2.y - p1.y, p2.x - p1.x) * 180.f / M_PI);
        window.draw(line);
    }

    if (isClosed && points.size() > 1) {
        sf::Vector2f p1 = points[0];
        sf::Vector2f p2 = points[points.size() - 1];

        line.setFillColor(sf::Color(color.r / 2.f, color.g / 2.f, color.b / 2.f));
        line.setSize(sf::Vector2f(std::sqrt(std::pow(p2.x - p1.x, 2) + std::pow(p2.y - p1.y, 2)), thickness));
        line.setOrigin(line.getSize() / 2.f);
        line.setPosition(sf::Vector2f((p1.x + p2.x) / 2.f, (p1.y + p2.y) / 2.f));
        line.setRotation(std::atan2(p2.y - p1.y, p2.x - p1.x) * 180.f / M_PI);
        window.draw(line);
    }
}

void drawCurve(sf::RenderWindow& window, const sf::VertexArray& curve)
{
    window.draw(curve);
}

void drawAll(sf::RenderWindow& window, const Bezier& bezier)
{
    const std::vector<sf::Vector2f>& points = bezier.getPoints();
    const std::vector<sf::Vector2f>& controlPoints = bezier.getControlPoints();
    const std::vector<sf::Vector2f>& anchorPoints = bezier.getAnchorPoints();
    const sf::VertexArray& curve = bezier.getBezierCurve();
    const bool isClosed = bezier.isClosed();

    drawControlLines(window, controlPoints, isClosed);
    drawPointLines(window, points, isClosed);

    drawPoints(window, points);
    drawControlPoints(window, controlPoints);
    drawAnchorPoints(window, anchorPoints);

    drawCurve(window, curve);
}


int main(int ac, char** av)
{
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Bezier");

    Bezier circle({
        sf::Vector2f(300, 50),
        sf::Vector2f(50, 300),
        sf::Vector2f(300, 550),
        sf::Vector2f(550, 300) },
        true
        );

    Bezier bezier;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                sf::Vector2i click(sf::Mouse::getPosition(window));

                bezier.addPoint(sf::Vector2f(click));
                bezier.update();
            }
        }

        window.clear();
        drawAll(window, circle);
        drawAll(window, bezier);
        window.display();
    }

    return 0;
}
*/