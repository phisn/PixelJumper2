#include <source\scene\setup\Setup.h>

Setup* Setup::setup = NULL;

Setup::Setup()
{
    userInterface = new tgui::Gui(*Window::renderWindow());
    SetupMenu::main = new SetupMainMenu(userInterface);
    SetupMenu::online = new SetupOnlineMenu(userInterface);

    SetupMenu::main->show();
}

void Setup::onShutdown()
{
}

void Setup::onEvent(
    sf::Event& event)
{
    if (isFront())
    {
        userInterface->handleEvent(event);
    }
}

void Setup::onLogic(
    sf::Time& time)
{
}

void Setup::onDraw()
{
    if (isFront())
    {
        userInterface->draw();
    }
}

Setup* Setup::instance()
{
    if (setup == NULL)
    {
        setup = new Setup();
    }

    return setup;
}
