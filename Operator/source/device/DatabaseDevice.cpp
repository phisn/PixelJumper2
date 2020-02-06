#include "DatabaseDevice.h"

#include <Client/source/logger/Logger.h>

#include <Operator/source/database/KeyTable.h>
#include <iostream>

namespace
{
	sqlite3* database;
}

namespace Device::Database
{
	bool FinalizeStatement(sqlite3_stmt* const statement);
	bool VerifyDatabase();

	bool Initialize()
	{
		int result = sqlite3_open16(L"pj2.db", &database);

		if (result)
		{
			 Log::Error(
				 std::wstring(L"Failed to open database (") +
				 (const wchar_t*) sqlite3_errmsg16(database) +
				L")",
				 result, L"result");

			 return false;
		}

		return VerifyDatabase();
	}

	bool VerifyDatabase()
	{
		::Database::KeyTable key;

		return true;
	}

	void Unintialize()
	{
		sqlite3_close(database);
	}

	bool Extract(::Database::TableBase* const table)
	{
		sqlite3_stmt* statement;
		int result = table->extract(database, &statement);

		if (result != SQLITE_OK)
		{
			Log::Error(L"failed to create select statement",
				result, L"result");

			return false;
		}

		result = sqlite3_step(statement);
		
		if (result != SQLITE_ROW)
		{
			if (result != SQLITE_DONE)
			{
				Log::Error(L"Failed to step statement in extraction",
					result, L"result");
			}

			FinalizeStatement(statement);

			return false;
		}
		else
		{
			table->apply(statement);
		}

		return FinalizeStatement(statement);
	}

	bool Insert(::Database::TableBase* const table)
	{
		sqlite3_stmt* statement;
		int result = table->create(database, &statement);

		if (result != SQLITE_OK)
		{
			Log::Error(L"Failed to create insert statement",
				result, L"result");

			return false;
		}

		result = sqlite3_step(statement);

		if (result != SQLITE_DONE)
		{
			Log::Error(L"Failed to step statement",
				result, L"result");

			return false;
		}

		return FinalizeStatement(statement);
	}

	bool Edit(::Database::TableBase* const table)
	{
		sqlite3_stmt* statement;
		int result = table->edit(database, &statement);

		if (result != SQLITE_OK)
		{
			Log::Error(L"Failed to create update statement",
				result, L"result");

			return false;
		}

		result = sqlite3_step(statement);

		if (result != SQLITE_DONE)
		{
			Log::Error(L"Failed to step statement",
				result, L"result");

			return false;
		}

		return FinalizeStatement(statement);
	}

	bool FinalizeStatement(sqlite3_stmt* const statement)
	{
		const int result = sqlite3_finalize(statement);

		if (result != SQLITE_OK)
		{
			Log::Error(L"Failed to finalize statement",
				result, L"result");

			return false;
		}

		return true;
	}

	sqlite3* GetConnection()
	{
		return database;
	}
}
