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

		virtual int create(sqlite3* const database, sqlite3_stmt** const statement)
		{
			std::stringstream ss;

			ss << "INSERT INTO ";
			ss << table;
			ss << "(";

			const ColumnValuesContainer allContainer = getAllColumnValues();
			for (const ColumnValuesPair& pair : allContainer)
			{
				ss << pair.first;
				
				if (pair != allContainer.back())
				{
					ss << ",";
				}
			}

			ss << ") VALUES (";

			for (const ColumnValuesPair& pair : allContainer)
			{
				ss << pair.second;

				if (pair != allContainer.back())
				{
					ss << ",";
				}
			}

			ss << ");";

			const std::string result = ss.str();

			return sqlite3_prepare_v2(
				database,
				result.c_str(),
				result.size(),
				statement,
				NULL);
		}

		virtual int edit(sqlite3* const database, sqlite3_stmt** const statement)
		{
			std::stringstream ss;

			ss << "UPDATE ";
			ss << table;
			ss << " SET ";

			const ColumnValuesContainer allContainer = getAllColumnValues();
			for (const ColumnValuesPair& pair : allContainer)
			{
				ss << pair.first;
				ss << "=";
				ss << pair.second;

				if (pair != allContainer.back())
				{
					ss << ",";
				}
			}

			ss << " WHERE ";
			
			const ColumnValuesContainer primaryKeyContainer = getPrimaryKeyColumnValues();
			for (const ColumnValuesPair& pair : primaryKeyContainer)
			{
				ss << pair.first;
				ss << "=";
				ss << pair.second;
				
				if (pair != primaryKeyContainer.back())
				{
					ss << " AND ";
				}
			}

			ss << ";";

			const std::string result = ss.str();
			
			return sqlite3_prepare_v2(
				database,
				result.c_str(),
				result.size(),
				statement,
				NULL);
		}

		virtual int extract(sqlite3* const database, sqlite3_stmt** const statement)
		{
			std::stringstream ss;

			ss << "SELECT * FROM ";
			ss << table;
			ss << " WHERE ";

			const ColumnValuesContainer primaryKeyContainer = getPrimaryKeyColumnValues();
			for (const ColumnValuesPair& pair : primaryKeyContainer)
			{
				ss << pair.first;
				ss << "=";
				ss << pair.second;

				if (pair != primaryKeyContainer.back())
				{
					ss << " AND ";
				}
			}

			ss << ";";

			const std::string result = ss.str();

			return sqlite3_prepare_v2(
				database,
				result.c_str(),
				result.size(),
				statement,
				NULL);
		}

		virtual void apply(sqlite3_stmt* const statement) = 0;

	protected:
		virtual const ColumnValuesContainer getAllColumnValues() = 0;
		virtual const ColumnValuesContainer getPrimaryKeyColumnValues() = 0;

	private:
		const std::string table;
	};
}
