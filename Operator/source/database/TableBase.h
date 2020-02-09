#pragma once

#include <sstream>
#include <string>
#include <vector>

#include <sqlite3.h>

namespace Database
{
	// need to rework tablebase because of sqlinjections
	// to prevent sqlinjections in sqlite you have to use
	// parameterized statements where these parameters 
	// can later be bound to as values with sepicific types
	// because we dont know the types anymore at statement
	// construction we have to rethink this whole structure.
	// a another important aspect is the efficiency with
	// larger tables and maybe the sepcialization of primary
	// keys. because we often dont need all information or
	// need it based on different conditions we have to
	// add a dynamic aspect to solve this problem. this would
	// also eliminate the problem of sqlbinding in
	// findstatements because they would be of no need
	// anymore
	
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

		int create(sqlite3* const database, sqlite3_stmt** const statement);
		int edit(sqlite3* const database, sqlite3_stmt** const statement);
		int extract(sqlite3* const database, sqlite3_stmt** const statement);

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
				"hash"	BLOB,
				"salt"	BLOB,
				PRIMARY KEY("id")
			);)__",
			R"__(
			CREATE TABLE "keys" (
				"key"	BLOB NOT NULL,
				"player"	INTEGER UNIQUE,
				"source"	TEXT,
				PRIMARY KEY("key"),
				FOREIGN KEY("player") REFERENCES "players"("id") ON DELETE SET NULL
			);)__"
		};

		const std::string table;
	};
}
