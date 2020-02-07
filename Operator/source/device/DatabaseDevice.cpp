#include "DatabaseDevice.h"

#include <Client/source/logger/Logger.h>

#include <Operator/source/database/EmptyKeyStatement.h>
#include <iostream>

namespace
{
	sqlite3* database;
}

namespace Device::Database
{
	bool VerifyDatabase();

	bool Initialize()
	{
		int result = sqlite3_open16(L"pj2.db", &database);

		if (result)
		{
			 Log::Error(L"Failed to open database " + GenerateErrorMessage(),
				 result, L"result");

			 return false;
		}

		return VerifyDatabase();
	}

	bool VerifyDatabase()
	{
		return true;
	}

	void Unintialize()
	{
		sqlite3_close(database);
	}

	ExtractionResult Extract(::Database::TableBase* const table)
	{
		sqlite3_stmt* statement;
		int result = table->extract(database, &statement);

		if (result != SQLITE_OK)
		{
			Log::Error(L"Failed to create select statement " + GenerateErrorMessage(),
				result, L"result");

			return ExtractionResult::Error;
		}

		result = sqlite3_step(statement);
		
		if (result != SQLITE_ROW)
		{
			FinalizeStatement(statement);

			if (result != SQLITE_DONE)
			{
				Log::Error(L"Failed to step statement in extraction " + GenerateErrorMessage(),
					result, L"result");

				return ExtractionResult::Error;
			}
			else
			{
				return ExtractionResult::NotFound;
			}
		}
		else
		{
			table->apply(statement);
		}

		return ExtractionResult::Found;
	}

	bool BeginTransaction()
	{
		const int result = sqlite3_exec(database, "BEGIN TRANSACTION", NULL, NULL, NULL);

		if (result == SQLITE_OK)
		{
			return true;
		}
		else
		{
			Log::Error(L"Failed to begin transaction " + GenerateErrorMessage(),
				result, L"result");

			return false;
		}
	}

	bool EndTransaction()
	{
		const int result = sqlite3_exec(database, "END TRANSACTION", NULL, NULL, NULL);

		if (result == SQLITE_OK)
		{
			return true;
		}
		else
		{
			Log::Error(L"Failed to end transaction " + GenerateErrorMessage(),
				result, L"result");

			return false;
		}
	}

	bool RollbackTransaction()
	{
		const int result = sqlite3_exec(database, "ROLLBACK TRANSACTION", NULL, NULL, NULL);

		if (result == SQLITE_OK)
		{
			return true;
		}
		else
		{
			Log::Error(L"Failed to rollback transaction " + GenerateErrorMessage(),
				result, L"result");

			return false;
		}
	}

	bool Exists(::Database::TableBase* const table)
	{
		return Extract(table) == ExtractionResult::Found;
	}

	bool Insert(::Database::TableBase* const table)
	{
		sqlite3_stmt* statement;
		int result = table->create(database, &statement);

		if (result != SQLITE_OK)
		{
			Log::Error(L"Failed to create insert statement " + GenerateErrorMessage(),
				result, L"result");

			return false;
		}

		result = sqlite3_step(statement);

		if (result != SQLITE_DONE)
		{
			Log::Error(L"Failed to step statement " + GenerateErrorMessage(),
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
			Log::Error(L"Failed to create update statement " + GenerateErrorMessage(),
				result, L"result");

			return false;
		}

		result = sqlite3_step(statement);

		if (result != SQLITE_DONE)
		{
			Log::Error(L"Failed to step statement " + GenerateErrorMessage(),
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
			Log::Error(L"Failed to finalize statement " + GenerateErrorMessage(),
				result, L"result");

			return false;
		}

		return true;
	}

	sqlite3* GetConnection()
	{
		return database;
	}

	std::wstring GenerateErrorMessage()
	{
		return L'(' + carrtowstr(sqlite3_errmsg(database)) + L')';
	}
}
