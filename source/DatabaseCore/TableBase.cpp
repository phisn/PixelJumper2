#include "TableBase.h"
#include "Logger/Logger.h"

Database::ConditionResult Database::TableBase::exists(const Columns condition) const
{
	std::stringstream ss;

	ss << "SELECT 1 FROM ";
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

	SQLiteStatement statement{ database };
	int result = sqlite3_prepare_v2(
		database->getSQLite(),
		command.c_str(),
		command.size(),
		&statement.statement,
		NULL);

	if (result != SQLITE_OK)
	{
		Log::Error(L"Failed to create select statement " + database->getCurrentSQLiteFailureMessage(),
			result, L"result");

		return ConditionResult::Error;
	}

	for (int i = 0; i < condition.size(); ++i)
		if (bindColumnValue(statement.statement, condition[i], i + 1) != SQLITE_OK)
		{
			return ConditionResult::Error;
		}

	result = sqlite3_step(statement.statement);
	if (result != SQLITE_ROW)
	{
		if (result == SQLITE_DONE)
			return ConditionResult::NotFound;

		Log::Error(L"Failed to step statement in extraction " + database->getCurrentSQLiteFailureMessage(),
			result, L"result");

		return ConditionResult::Error;
	}

	return ConditionResult::Found;
}

bool Database::TableBase::create()
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

	SQLiteStatement statement{ database };
	int result = sqlite3_prepare_v2(
		database->getSQLite(),
		command.c_str(),
		command.size(),
		&statement.statement,
		NULL);

	if (result != SQLITE_OK)
	{
		Log::Error(L"Failed to create select statement " + database->getCurrentSQLiteFailureMessage(),
			result, L"result");

		return false;
	}

	for (int i = 0; i < definition->columns.size(); ++i)
		if (bindColumnValue(statement.statement, i, i + 1) != SQLITE_OK)
		{
			return false;
		}

	result = sqlite3_step(statement.statement);
	if (result != SQLITE_DONE)
	{
		Log::Error(L"Failed to step statement in extraction " + database->getCurrentSQLiteFailureMessage(),
			result, L"result");

		return false;
	}

	return true;
}

Database::ConditionResult Database::TableBase::edit(
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

	SQLiteStatement statement{ database };
	int result = sqlite3_prepare_v2(
		database->getSQLite(),
		command.c_str(),
		command.size(),
		&statement.statement,
		NULL);

	if (result != SQLITE_OK)
	{
		Log::Error(L"Failed to create select statement " + database->getCurrentSQLiteFailureMessage(),
			result, L"result");

		return ConditionResult::Error;
	}

	for (int i = 0; i < info.size(); ++i)
		if (bindColumnValue(statement.statement, info[i], i + 1) != SQLITE_OK)
		{
			return ConditionResult::Error;
		}

	// need to add info as offset
	for (int i = 0; i < condition.size(); ++i)
		if (bindColumnValue(statement.statement, condition[i], info.size() + i + 1) != SQLITE_OK)
		{
			return ConditionResult::Error;
		}

	result = sqlite3_step(statement.statement);
	if (result != SQLITE_DONE)
	{
		Log::Error(L"Failed to step statement in extraction " + database->getCurrentSQLiteFailureMessage(),
			result, L"result");

		return ConditionResult::Error;
	}

	return sqlite3_changes(database->getSQLite()) == 0
		? ConditionResult::NotFound
		: ConditionResult::Found;
}

Database::ConditionResult Database::TableBase::extract(
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

	SQLiteStatement statement{ database };
	int result = sqlite3_prepare_v2(
		database->getSQLite(),
		command.c_str(),
		command.size(),
		&statement.statement,
		NULL);

	if (result != SQLITE_OK)
	{
		Log::Error(L"Failed to create select statement " + database->getCurrentSQLiteFailureMessage(),
			result, L"result");

		return ConditionResult::Error;
	}

	for (int i = 0; i < condition.size(); ++i)
		if (bindColumnValue(statement.statement, condition[i], i + 1) != SQLITE_OK)
		{
			return ConditionResult::Error;
		}

	result = sqlite3_step(statement.statement);
	if (result != SQLITE_ROW)
	{
		if (result == SQLITE_DONE)
			return ConditionResult::NotFound;

		Log::Error(L"Failed to step statement in extraction " + database->getCurrentSQLiteFailureMessage(),
			result, L"result");

		return ConditionResult::Error;
	}

	const bool apply_result = apply(statement.statement, info);
	return apply_result
		? ConditionResult::Found
		: ConditionResult::Error;
}
