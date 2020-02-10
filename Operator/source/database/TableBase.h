#pragma once

#include <Operator/source/device/DatabaseDevice.h>

#include <sqlite3.h>

#include <sstream>
#include <string>
#include <vector>

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

	struct TableDefinition
	{
		std::string name;
		std::vector<std::string> columns;
	};

	enum class ConditionResult
	{
		Found,
		NotFound,
		Error
	};

	//  think about adding attribute as a class
	
	class _TableBase
	{
	public:
		typedef int ColumnIndex;
		typedef std::vector<ColumnIndex> Columns;

		_TableBase(const TableDefinition* definition)
			:
			definition(definition)
		{
		}

		bool create()
		{
			std::stringstream ss;

			ss << "INSERT INTO ";
			ss << definition->name;
			ss << " (";

			for (int i = 0; i < definition->columns.size() - 1; ++i)
			{
				ss << definition->columns[i];
				ss << ",";
			}

			ss << definition->columns.back();
			ss << ") VALUES (";

			for (int i = 0; i < definition->columns.size() - 1; ++i)
			{
				ss << "?,";
			}

			ss << "?);";


			const std::string command = ss.str();

			Log::Information(
				L"Executing statement '"
				+ carrtowstr(command.c_str())
				+ L"'");

			sqlite3_stmt* statement;
			int result = sqlite3_prepare_v2(
				Device::Database::GetConnection(),
				command.c_str(),
				command.size(),
				&statement,
				NULL);

			if (result != SQLITE_OK)
			{
				Log::Error(L"Failed to create select statement " + Device::Database::GenerateErrorMessage(),
					result, L"result");

				return false;
			}

			for (int i = 0; i < definition->columns.size(); ++i)
			{
				bindColumnValue(statement, i, i);
			}

			result = sqlite3_step(statement);
			Device::Database::FinalizeStatement(statement);

			if (result != SQLITE_DONE)
			{
				Log::Error(L"Failed to step statement in extraction " + Device::Database::GenerateErrorMessage(),
					result, L"result");

				return false;
			}

			return true;
		}

		ConditionResult edit(
			const Columns info,
			const Columns condition)
		{
			std::stringstream ss;

			ss << "UPDATE ";
			ss << definition->name;
			ss << " SET ";

			for (int index : info)
			{
				ss << definition->columns[index];
				ss << "=?";

				if (index != info.back())
					ss << ",";
			}

			ss << " WHERE ";

			for (int index : condition)
			{
				ss << definition->columns[index];
				ss << "=?";

				if (index != condition.back())
					ss << " AND ";
			}

			ss << ";";

			const std::string command = ss.str();

			Log::Information(
				L"Executing statement '"
				+ carrtowstr(command.c_str())
				+ L"'");

			sqlite3_stmt* statement;
			int result = sqlite3_prepare_v2(
				Device::Database::GetConnection(),
				command.c_str(),
				command.size(),
				&statement,
				NULL);

			if (result != SQLITE_OK)
			{
				Log::Error(L"Failed to create select statement " + Device::Database::GenerateErrorMessage(),
					result, L"result");

				return ConditionResult::Error;
			}

			for (int i = 0; i < condition.size(); ++i)
			{
				bindColumnValue(statement, condition[i], i);
			}

			result = sqlite3_step(statement);

			if (result != SQLITE_DONE)
			{
				Device::Database::FinalizeStatement(statement);

				Log::Error(L"Failed to step statement in extraction " + Device::Database::GenerateErrorMessage(),
					result, L"result");

				return ConditionResult::Error;
			}

			if (sqlite3_changes(Device::Database::GetConnection()) == 0)
			{
				return ConditionResult::NotFound;
			}

			Device::Database::FinalizeStatement(statement);

			return ConditionResult::Found;
		}

		ConditionResult extract(
			const Columns info,
			const Columns condition)
		{
			std::stringstream ss;

			ss << "SELECT ";

			for (int index : info)
			{
				ss << definition->columns[index];

				if (index != info.back())
					ss << ",";
			}

			ss << " FROM ";
			ss << definition->name;
			ss << " WHERE ";

			for (int index : condition)
			{
				ss << definition->columns[index];
				ss << "=?";

				if (index != condition.back())
					ss << " AND ";
			}

			ss << ';';

			const std::string command = ss.str();

			Log::Information(
				L"Executing statement '" 
				+ carrtowstr(command.c_str())
				+ L"'");

			sqlite3_stmt* statement;
			int result = sqlite3_prepare_v2(
				Device::Database::GetConnection(),
				command.c_str(),
				command.size(),
				&statement,
				NULL);

			if (result != SQLITE_OK)
			{
				Log::Error(L"Failed to create select statement " + Device::Database::GenerateErrorMessage(),
					result, L"result");

				return ConditionResult::Error;
			}

			for (int i = 0; i < condition.size(); ++i)
			{
				bindColumnValue(statement, condition[i], i);
			}

			result = sqlite3_step(statement);

			if (result != SQLITE_ROW)
			{
				Device::Database::FinalizeStatement(statement);

				if (result == SQLITE_DONE)
				{
					return ConditionResult::NotFound;
				}

				Log::Error(L"Failed to step statement in extraction " + Device::Database::GenerateErrorMessage(),
					result, L"result");

				return ConditionResult::Error;
			}

			for (int i = 0; i < info.size(); ++i)
			{
				adoptColumnValue(statement, info[i], i);
			}

			Device::Database::FinalizeStatement(statement);

			return ConditionResult::Found;
		}

	protected:
		virtual void bindColumnValue(
			sqlite3_stmt* const statement,
			const int column,
			const int columnIndex) = 0;
		virtual void adoptColumnValue(
			sqlite3_stmt* const statement,
			const int column,
			const int columnIndex) = 0;

	private:
		const TableDefinition* definition;
	};
	
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
