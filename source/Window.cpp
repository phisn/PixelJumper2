#include "Window.h"

sf::RenderWindow* Window::window = new sf::RenderWindow();

Window::Window()
{
    window = new sf::RenderWindow();
}

Window::~Window()
{
    delete window;
}

void Window::create()
{
    sf::Uint32 style;

    if (isFullScreen)
    {
        style = sf::Style::Fullscreen;
    }
    else
    {
        style = sf::Style::Close | sf::Style::Titlebar;
    }

    contextSettings.antialiasingLevel = 16;
    window->create(
        videMode,
        "Snake",
        std::move(style),
        contextSettings);

    // VERY IMPORTANT, DO NOT REMOVE THIS !!!
    window->setFramerateLimit(60);
    window->setVerticalSyncEnabled(true);
}

void Window::setVideMode(sf::VideoMode videMode)
{
    this->videMode = videMode;
}

sf::VideoMode& Window::getVideMode()
{
    return videMode;
}

sf::ContextSettings& Window::getContextSettings()
{
    return contextSettings;
}

void Window::setFullScreen(
    bool fullScreen)
{
    isFullScreen = fullScreen;
}

sf::RenderWindow* Window::renderWindow()
{
    return window;
}
