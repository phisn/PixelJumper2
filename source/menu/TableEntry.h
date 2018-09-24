#pragma once

#include <TGUI\TGUI.hpp>

class TableEntry
{
public:
    TableEntry();
    ~TableEntry();

    tgui::Layout getTotalWidth();
    tgui::Layout getTotalHeight();
private:
    tgui::HorizontalLayout hlayout;
    tgui::VerticalLayout vlayout;
};

