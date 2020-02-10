#include "TableBase.h"
#include <Client/source/logger/Logger.h>

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
		if (bindColumnValue(statement, condition[i], i + 1) != SQLITE_OK)
		{
			Device::Database::FinalizeStatement(statement);

			return ConditionResult::Error;
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

	Device::Database::FinalizeStatement(statement);

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
		if (bindColumnValue(statement, i, i + 1) != SQLITE_OK)
		{
			Device::Database::FinalizeStatement(statement);

			return false;
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

	for (int i = 0; i < info.size(); ++i)
		if (bindColumnValue(statement, info[i], i + 1) != SQLITE_OK)
		{
			Device::Database::FinalizeStatement(statement);

			return ConditionResult::Error;
		}

	// need to add info as offset
	for (int i = 0; i < condition.size(); ++i)
		if (bindColumnValue(statement, condition[i], info.size() + i + 1) != SQLITE_OK)
		{
			Device::Database::FinalizeStatement(statement);

			return ConditionResult::Error;
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
		if (bindColumnValue(statement, condition[i], i + 1) != SQLITE_OK)
		{
			Device::Database::FinalizeStatement(statement);

			return ConditionResult::Error;
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

	const bool apply_result = apply(statement, info);
	Device::Database::FinalizeStatement(statement);
	
	return apply_result
		? ConditionResult::Found
		: ConditionResult::Error;
}
