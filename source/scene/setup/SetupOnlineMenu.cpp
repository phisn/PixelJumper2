#include <source\scene\setup\SetupOnlineMenu.h>
#include <source\scene\setup\SetupMenuContainer.h>

SetupOnlineMenu::SetupOnlineMenu(
    tgui::Gui* userInterface)
    :
    MenuBase(userInterface)
{
    tgui::Grid::Ptr grid = tgui::Grid::create();

    grid->addWidget(makeHostButton(), 0, 0);
    grid->addWidget(makeJoinButton(), 1, 0);
    grid->addWidget(makeBackButton(), 2, 0);

    grid->setPosition(
        (windowWidth.getValue() / 2) - (grid->getSize().x / 2),
        (windowHeight.getValue() / 2) - (grid->getSize().y / 2));
    addWidget(grid);
}

tgui::Button::Ptr SetupOnlineMenu::makeHostButton()
{
    tgui::Button::Ptr button = tgui::Button::create("Host");

    button->setSize(
        windowWidth * MIDDLE_SIZE(40.f),
        windowHeight * 0.12f);
    button->connect("pressed", [this]()
    {
        SetupMenu::host->show();
    });

    return button;
}

tgui::Button::Ptr SetupOnlineMenu::makeJoinButton()
{
    tgui::Button::Ptr button = tgui::Button::create("Join");

    button->setSize(
        windowWidth * MIDDLE_SIZE(40.f),
        windowHeight * 0.12f);
    button->connect("pressed", [this]()
    {

    });

    return button;
}

tgui::Button::Ptr SetupOnlineMenu::makeBackButton()
{
    tgui::Button::Ptr button = tgui::Button::create("Back");

    button->setSize(
        windowWidth * MIDDLE_SIZE(40.f),
        windowHeight * 0.12f);
    button->connect("pressed", [this]()
    {
        SetupMenu::main->show();
    });

    return button;
}
