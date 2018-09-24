#pragma once

#include <TGUI\TGUI.hpp>

#include "IMenuEntry.h"

#define MIDDLE_SIZE(p) (p / 100.f)
#define MIDDLE_POSI(p) ((1.f - MIDDLE_SIZE(p)) / 2.f)

class Menu;
struct MenuEntry
{
    template<class T>
    T getParsedWidget()
    {
        return static_cast<T>(widget.get());
    }

    tgui::ClickableWidget::Ptr widget;

    float max_height_procent = 15.f;
    float window_padding_procent = 5.f;
    float widget_margin_procent = 4.f;

    float width_position_procent = 40.f;
    float width_procent = 40.f;
private:
    void calculateLayout();

    void applyPosition();
    void applySize();

    tgui::Layout sizeHeight, positionHeight;

    friend Menu;
    Menu* menu;
};

class Menu
{
public:
    Menu(
        tgui::Gui& userInterface,
        unsigned int size);
    ~Menu();

    void show();
    void pushMenuEntry(MenuEntry* menuEntry);

    void skipWidget(float count = 1.f)
    {
        widgetCounter += count;
    }
private:
    friend MenuEntry;

    const unsigned int size;
    float widgetCounter = 0;

    tgui::Gui& userInterface;
    std::vector<MenuEntry*> menuEntries;

    tgui::Layout windowWidth;
    tgui::Layout windowHeight;
};

