#include <source\menu\TableLayout.h>

TableLayout::TableLayout(
    const int columns,
    const int rows)
    :
    columns(columns),
    rows(rows),
    widgetTable(columns),
    maxColumnWidth(rows),
    maxRowHeight(columns)
{
    for (std::vector<TableEntry>& row : widgetTable)
    {
        row.resize(rows);
    }
}

void TableLayout::addWidget(
    TableEntry widget,
    const int column,
    const int row)
{
    widgetTable[column][row] = widget;
}

void TableLayout::calculateMaxColumnWidth()
{
    for (int column = 0; column < columns; ++column)
    {
        maxColumnWidth[column] = widgetTable[0][0].getTotalWidth();

        for (int row = 1; row < rows; ++row)
        {
            tgui::Layout width = widgetTable[column][row].getTotalWidth();

            if (width.getValue() > maxColumnWidth[column].getValue())
            {
                maxColumnWidth[column] = std::move(width);
            }
        }
    }
}

void TableLayout::calcluateMaxRowHeight()
{
    for (int row = 0; row < rows; ++row)
    {
        maxRowHeight[row] = widgetTable[0][0].getTotalHeight();

        for (int column = 1; column < columns; ++column)
        {
            tgui::Layout height = widgetTable[column][row].getTotalHeight();

            if (height.getValue() > maxRowHeight[row].getValue())
            {
                maxRowHeight[row] = std::move(height);
            }
        }
    }
}
