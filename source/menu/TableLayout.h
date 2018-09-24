#pragma once

#include <source\menu\TableEntry.h>
#include <TGUI\TGUI.hpp>

class TableLayout
{
public:
    TableLayout(
        const int columns,
        const int rows);

    void addWidget(
        TableEntry widget,
        const int column,
        const int row);
private:
    void calculateMaxColumnWidth();
    void calcluateMaxRowHeight();

    const int columns, rows;
    std::vector<tgui::Layout> maxColumnWidth;
    std::vector<tgui::Layout> maxRowHeight;

    std::vector<std::vector<TableEntry>> widgetTable;
};

