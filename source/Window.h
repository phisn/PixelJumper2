#pragma once

#include <SFML\Graphics.hpp>

class Window
{
public:
    Window();
    ~Window();

    void create();

    void setVideMode(sf::VideoMode videMode);
    sf::VideoMode& getVideMode();
    sf::ContextSettings& getContextSettings();
    void setFullScreen(
        bool fullScreen);

    const bool isAlive() const
    {
        return window->isOpen();
    }
   
    static sf::RenderWindow* renderWindow();
private:
    static sf::RenderWindow* window;

    sf::VideoMode videMode;
    sf::ContextSettings contextSettings;
    bool isFullScreen;
};

