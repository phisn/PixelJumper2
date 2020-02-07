#pragma once

#include <sstream>
#include <string>
#include <vector>

#include <sqlite3.h>

namespace Database
{
	class TableBase
	{
	public:
		typedef std::pair<std::string, std::string> ColumnValuesPair;
		typedef std::vector<ColumnValuesPair> ColumnValuesContainer;

		inline static std::vector<std::string> getTableCreation()
		{
			const int count = sizeof(CreateTable) / sizeof(*CreateTable);
			std::vector<std::string> buffer(count);

			for (int i = 0; i < count; ++i)
			{
				buffer.push_back(CreateTable[i]);
			}

			return buffer;
		}

		TableBase(const std::string table)
			:
			table(table)
		{
		}

		virtual int create(sqlite3* const database, sqlite3_stmt** const statement);
		virtual int edit(sqlite3* const database, sqlite3_stmt** const statement);
		virtual int extract(sqlite3* const database, sqlite3_stmt** const statement);

		virtual void apply(sqlite3_stmt* const statement) = 0;

		virtual const ColumnValuesContainer getAllColumnValues() = 0;
		virtual const ColumnValuesContainer getPrimaryKeyColumnValues() = 0;

		const std::string getTableName()
		{
			return table;
		}

	private:
		constexpr static const char* CreateTable[] =
		{
			R"__(
			CREATE TABLE "players" (
				"id"	INTEGER NOT NULL,
				"password"	BLOB,
				"salt"	BLOB,
				PRIMARY KEY("id")
			);)__",
			R"__(
			CREATE TABLE "keys" (
				"key"	BLOB NOT NULL,
				"player"	INTEGER UNIQUE,
				"source"	INTEGER,
				PRIMARY KEY("key"),
				FOREIGN KEY("player") REFERENCES "players"("id") ON DELETE SET NULL
			);)__"
		};

		const std::string table;
	};
}
