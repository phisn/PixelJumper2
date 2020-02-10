#include "DatabaseDevice.h"

#include <Client/source/logger/Logger.h>

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
