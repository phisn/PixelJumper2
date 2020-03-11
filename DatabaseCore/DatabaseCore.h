#pragma once

#include "Logger/Logger.h"

#include <sqlite3.h>

namespace Database
{
	// used in tablebase
	enum class ConditionResult
	{
		Found,
		NotFound,
		Error
	};

	class SQLiteDatabase
	{
	public:
		~SQLiteDatabase()
		{
			if (database)
				close();
		}

		bool open(const std::wstring file)
		{
			this->file = file;
			lastSQLiteFailure = sqlite3_open16((const void*) file.c_str(), &database);

			if (lastSQLiteFailure)
			{
				Log::Error(L"Failed to open database" + getCurrentSQLiteFailureMessage(),
					file, L"file",
					lastSQLiteFailure, L"result");

				return false;
			}

			return verifyDatabase();
		}

		bool close()
		{
			lastSQLiteFailure = sqlite3_close(database);
			database = NULL;

			if (lastSQLiteFailure)
			{
				Log::Error(L"Failed to open database" + getCurrentSQLiteFailureMessage(),
					file, L"file",
					lastSQLiteFailure, L"result");

				return false;
			}

			file = L"";
			return lastSQLiteFailure == 0;
		}

		bool beginTransaction()
		{
			lastSQLiteFailure = sqlite3_exec(
				database, 
				"BEGIN TRANSACTION", 
				NULL, 
				NULL, 
				NULL);

			if (lastSQLiteFailure)
			{
				Log::Error(L"Failed to begin transaction " + getCurrentSQLiteFailureMessage(),
					lastSQLiteFailure, L"result");

				return false;
			}

			Log::Information(L"Begin transaction");
			return true;
		}

		bool endTransaction()
		{
			lastSQLiteFailure = sqlite3_exec(
				database,
				"END TRANSACTION",
				NULL,
				NULL,
				NULL);

			if (lastSQLiteFailure)
			{
				Log::Error(L"Failed to end transaction " + getCurrentSQLiteFailureMessage(),
					lastSQLiteFailure, L"result");

				return false;
			}

			Log::Information(L"End transaction");
			return true;
		}

		bool rollbackTransaction()
		{
			lastSQLiteFailure = sqlite3_exec(
				database,
				"ROLLBACK TRANSACTION",
				NULL,
				NULL,
				NULL);

			if (lastSQLiteFailure)
			{
				Log::Error(L"Failed to rollback transaction " + getCurrentSQLiteFailureMessage(),
					lastSQLiteFailure, L"result");

				return false;
			}

			Log::Information(L"Rollback transaction");
			return true;
		}
		
		const std::wstring& getFileName() const
		{
			return file;
		}

		sqlite3* getSQLite() const
		{
			return database;
		}

		std::wstring getCurrentSQLiteFailureMessage() const
		{
			return L'\'' + carrtowstr(sqlite3_errmsg(database)) + L'\'';
		}

		int getLastSQLiteFailure() const
		{
			return lastSQLiteFailure;
		}

	protected:
		int lastSQLiteFailure = 0;

		virtual bool verifyDatabase()
		{
			return true;
		}

		sqlite3* database = NULL;
		std::wstring file;
	};

	class SQLiteStatement
	{
		friend class StatementBase;

	public:
		~SQLiteStatement()
		{
			if (statement)
				finalize();
		}

		SQLiteStatement(SQLiteDatabase* const parent)
			:
			parent(parent)
		{
		}

		sqlite3_stmt* statement = NULL;

		bool finalize()
		{
			const int result = sqlite3_finalize(statement);

			if (result)
			{
				Log::Error(L"Failed to finalize statement " + parent->getCurrentSQLiteFailureMessage(),
					result, L"result");
			}

			statement = NULL;
			return result == SQLITE_OK;
		}

	private:
		SQLiteDatabase* const parent;
	};
}
