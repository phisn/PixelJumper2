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
		static bool Initialize(const std::wstring file)
		{
			operatorDatabase = new OperatorDatabase();
			const bool result = operatorDatabase->open(file);

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
