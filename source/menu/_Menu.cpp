#include "Menu.h"

void MenuEntry::calculateLayout()
{
    tgui::Layout windowPadding = menu->windowHeight * window_padding_procent / 100;
    tgui::Layout buttonHeight = (menu->windowHeight - windowPadding * 2) / menu->size;

    tgui::Layout max_height = menu->windowHeight * max_height_procent / 100;
    if (buttonHeight.getValue() > max_height.getValue())
    {
        buttonHeight = max_height;
        windowPadding = (menu->windowHeight - (buttonHeight * menu->size)) / 2;
    }

    sizeHeight = buttonHeight - buttonHeight * widget_margin_procent / 100;
    positionHeight = windowPadding + buttonHeight * menu->widgetCounter;
}

void MenuEntry::applyPosition()
{
    widget->setPosition(
        menu->windowWidth * MIDDLE_POSI(width_position_procent),
        positionHeight);
}

void MenuEntry::applySize()
{
    widget->setSize(
        menu->windowWidth * MIDDLE_SIZE(width_procent),
        sizeHeight);
}

Menu::Menu(
    tgui::Gui& userInterface, 
    unsigned int size)
    :
    size(size),
    userInterface(userInterface)
{
    windowWidth = tgui::bindWidth(userInterface);
    windowHeight = tgui::bindHeight(userInterface);
}

Menu::~Menu()
{
    for (MenuEntry* menuEntry : menuEntries)
    {
        delete menuEntry;
    }
}

void Menu::show()
{
    userInterface.removeAllWidgets();

    for (MenuEntry* menuEntry : menuEntries)
    {
        userInterface.add(menuEntry->widget);
    }
}

void Menu::pushMenuEntry(MenuEntry* menuEntry)
{
    menuEntry->menu = this;

    menuEntry->calculateLayout();
    menuEntry->applyPosition();
    menuEntry->applySize();

    menuEntries.push_back(menuEntry);
    ++widgetCounter;
}
