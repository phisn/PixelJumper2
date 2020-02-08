#include "TableBase.h"
#include <Client/source/logger/Logger.h>

int Database::TableBase::create(sqlite3* const database, sqlite3_stmt** const statement)
{
	std::stringstream ss;

	ss << "INSERT INTO ";
	ss << table;
	ss << " (";

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

	Log::Information(L"Executing " + carrtowstr(result.c_str()));

	return sqlite3_prepare_v2(
		database,
		result.c_str(),
		(int) result.size(),
		statement,
		NULL);
}

int Database::TableBase::edit(sqlite3* const database, sqlite3_stmt** const statement)
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

	Log::Information(L"Executing " + carrtowstr(result.c_str()));

	return sqlite3_prepare_v2(
		database,
		result.c_str(),
		(int) result.size(),
		statement,
		NULL);
}

int Database::TableBase::extract(sqlite3* const database, sqlite3_stmt** const statement)
{
	std::stringstream ss;

	ss << "SELECT ";

	const ColumnValuesContainer allContainer = getAllColumnValues();
	for (const ColumnValuesPair& pair : allContainer)
	{
		ss << pair.first;

		if (pair != allContainer.back())
		{
			ss << ",";
		}
	}

	ss << " FROM ";
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

	Log::Information(L"Executing " + carrtowstr(result.c_str()));

	return sqlite3_prepare_v2(
		database,
		result.c_str(),
		(int) result.size(),
		statement,
		NULL);
}