#include <source\scene\setup\SetupMainMenu.h>
#include <source\scene\setup\SetupMenuContainer.h>

SetupMainMenu::SetupMainMenu(
    tgui::Gui* userInterface)
    :
    MenuBase(userInterface)
{
    addWidget(makeLabelButton());
    addWidget(makeOnlineButton());
    addWidget(makeOfflineButton());
    addWidget(makeSettingsButton());
    addWidget(makeExitButton());
}

tgui::Label::Ptr&& SetupMainMenu::makeLabelButton()
{
    tgui::Label::Ptr label = tgui::Label::create("Snake Multiplayer");

    label->setPosition(
        windowWidth * MIDDLE_POSI(40.f),
        windowHeight * 0.1f);
    label->setSize(
        windowWidth * MIDDLE_SIZE(40.f),
        windowHeight * 0.1f);

    return std::move(label);
}

tgui::Button::Ptr&& SetupMainMenu::makeOnlineButton()
{
    tgui::Button::Ptr button = tgui::Button::create("Online");

    button->setPosition(
        windowWidth * MIDDLE_POSI(40.f),
        windowHeight * 0.25f);
    button->setSize(
        windowWidth * MIDDLE_SIZE(40.f),
        windowHeight * 0.1f);
    button->connect("pressed", [this]()
    {
        SetupMenu::online->show();
    });

    return std::move(button);
}

tgui::Button::Ptr&& SetupMainMenu::makeOfflineButton()
{
    tgui::Button::Ptr button = tgui::Button::create("Online");

    button->setPosition(
        windowWidth * MIDDLE_POSI(40.f),
        windowHeight * 0.4f);
    button->setSize(
        windowWidth * MIDDLE_SIZE(40.f),
        windowHeight * 0.1f);
    button->connect("pressed", [this]()
    {

    });

    return std::move(button);
}

tgui::Button::Ptr&& SetupMainMenu::makeSettingsButton()
{
    tgui::Button::Ptr button = tgui::Button::create("Online");

    button->setPosition(
        windowWidth * MIDDLE_POSI(40.f),
        windowHeight * 0.55f);
    button->setSize(
        windowWidth * MIDDLE_SIZE(40.f),
        windowHeight * 0.1f);
    button->connect("pressed", [this]()
    {

    });

    return std::move(button);
}

tgui::Button::Ptr&& SetupMainMenu::makeExitButton()
{
    tgui::Button::Ptr button = tgui::Button::create("Online");

    button->setPosition(
        windowWidth * MIDDLE_POSI(40.f),
        windowHeight * 0.7f);
    button->setSize(
        windowWidth * MIDDLE_SIZE(40.f),
        windowHeight * 0.1f);
    button->connect("pressed", [this]()
    {
        SceneHandler::instance()->popScene();
    });

    return std::move(button);
}