#pragma once

#include <sstream>
#include <string>
#include <vector>

#include <sqlite3.h>

namespace Database
{
	class TableBase
	{
	protected:
		typedef std::pair<std::string, std::string> ColumnValuesPair;
		typedef std::vector<ColumnValuesPair> ColumnValuesContainer;

	public:
		TableBase(const std::string table)
			:
			table(table)
		{
		}

		virtual int create(sqlite3* const database, sqlite3_stmt** const statement);
		virtual int edit(sqlite3* const database, sqlite3_stmt** const statement);
		virtual int extract(sqlite3* const database, sqlite3_stmt** const statement);

		virtual void apply(sqlite3_stmt* const statement) = 0;

	protected:
		virtual const ColumnValuesContainer getAllColumnValues() = 0;
		virtual const ColumnValuesContainer getPrimaryKeyColumnValues() = 0;

	private:
		const std::string table;
	};
}
