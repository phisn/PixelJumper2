#pragma once

#include "Logger/Logger.h"

#include <functional>
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

		bool open(const std::string file)
		{
			this->file = file;
			lastSQLiteFailure = sqlite3_open(file.c_str(), &database);

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

			file = "";
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

		bool attach(std::string filename, std::string alias)
		{
			std::string statement = "ATTACH DATABASE '+" + filename + "' AS " + alias;
			lastSQLiteFailure = sqlite3_exec(
				database,
				statement.c_str(),
				NULL,
				NULL,
				NULL);

			if (lastSQLiteFailure)
			{
				Log::Error(L"Failed to attach database " + getCurrentSQLiteFailureMessage(),
					lastSQLiteFailure, L"result");

				return false;
			}

			Log::Information(L"Database attached",
				filename, L"file",
				alias, L"as");

			return true;
		}
		
		const std::string& getFileName() const
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
		std::string file;
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

	template <typename Director, typename Statement>
	int ExecuteStatement(
		SQLiteDatabase* database, 
		Director& director,
		std::function<void()> callback)
	{
		Statement statement;
		int result = director.execute(statement);

		if (result != SQLITE_OK)
		{
			Log::Error(L"Failed to execute statement " + database->getCurrentSQLiteFailureMessage(),
				result, L"result");

			return result;
		}

		while (true)
		{
			result = director.next(statement);

			if (result != SQLITE_ROW)
			{
				break;
			}

			callback();
		}

		if (result != SQLITE_DONE)
		{
			Log::Error(L"Failed to process statement " + database->getCurrentSQLiteFailureMessage(),
				result, L"result");
		}

		return result;
	}
}
