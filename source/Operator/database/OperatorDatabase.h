#pragma once

#include "DatabaseCore/DatabaseCore.h"

namespace Operator
{
	class OperatorDatabase
		:
		public Database::SQLiteDatabase
	{
		static OperatorDatabase* operatorDatabase;

	public:
		static bool Initialize(
			std::string file, 
			std::string threatFile)
		{
			operatorDatabase = new OperatorDatabase();
			bool result = operatorDatabase->open(file);

			if (result)
			{
				result = operatorDatabase->attach(threatFile, "tdb");
			}

			if (!result)
			{
				Log::Error(L"Failed to initialize operator database",
					file, L"file",
					operatorDatabase->getCurrentSQLiteFailureMessage(), L"errormessage",
					operatorDatabase->lastSQLiteFailure, L"result");

				delete operatorDatabase;
			}

			return result;
		}

		static bool Uninitialize()
		{
			const bool result = operatorDatabase->close();
			delete operatorDatabase;
			return result;
		}

		static std::wstring GetErrorMessage()
		{
			return operatorDatabase->getCurrentSQLiteFailureMessage();
		}

		static SQLiteDatabase* GetSQLiteDatabase()
		{
			return operatorDatabase;
		}
	};

	struct OperatorDatabaseStatement
		:
		Database::SQLiteStatement
	{
		OperatorDatabaseStatement()
			:
			Database::SQLiteStatement{ OperatorDatabase::GetSQLiteDatabase() }
		{
		}
	};
}
