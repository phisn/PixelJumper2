#include <source\scene\setup\SetupMainMenu.h>
#include <source\scene\setup\SetupMenuContainer.h>

SetupMainMenu::SetupMainMenu(
    tgui::Gui* userInterface)
    :
    MenuBase(userInterface)
{
    tgui::Grid::Ptr grid = tgui::Grid::create();

    grid->addWidget(makeLabel(), 0, 0);

    grid->addWidget(makeOnlineButton(), 1, 0);
    grid->addWidget(makeOfflineButton(), 2, 0);
    grid->addWidget(makeSettingsButton(), 3, 0);
    grid->addWidget(makeExitButton(), 4, 0);

    grid->setPosition(
        (windowWidth.getValue() / 2) - (grid->getSize().x / 2),
        (windowHeight.getValue() / 2) - (grid->getSize().y / 2));

    addWidget(grid);
}

tgui::Label::Ptr SetupMainMenu::makeLabel()
{
    tgui::Label::Ptr label = tgui::Label::create("Snake Multiplayer");

    label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
    label->setSize(windowWidth * 0.5, windowHeight * 0.2);
    label->setTextSize(100);
    label->setTextColor(sf::Color::White);

    return label;
}

tgui::Button::Ptr SetupMainMenu::makeOnlineButton()
{
    tgui::Button::Ptr button = tgui::Button::create("Online");

    button->setSize(
        windowWidth * MIDDLE_SIZE(40.f),
        windowHeight * 0.12f);
    button->connect("pressed", [this]()
    {
        SetupMenu::online->show();
    });

    return button;
}

tgui::Button::Ptr SetupMainMenu::makeOfflineButton()
{
    tgui::Button::Ptr button = tgui::Button::create("Offline");

    button->setSize(
        windowWidth * MIDDLE_SIZE(40.f),
        windowHeight * 0.12f);
    button->connect("pressed", [this]()
    {

    });

    return button;
}

tgui::Button::Ptr SetupMainMenu::makeSettingsButton()
{
    tgui::Button::Ptr button = tgui::Button::create("Settings");

    button->setSize(
        windowWidth * MIDDLE_SIZE(40.f),
        windowHeight * 0.12f);
    button->connect("pressed", [this]()
    {

    });

    return button;
}

tgui::Button::Ptr SetupMainMenu::makeExitButton()
{
    tgui::Button::Ptr button = tgui::Button::create("Exit");

    button->setSize(
        windowWidth * MIDDLE_SIZE(40.f),
        windowHeight * 0.12f);
    button->connect("pressed", [this]()
    {
        SceneHandler::instance()->popScene();
    });

    return button;
}