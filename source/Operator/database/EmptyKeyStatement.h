#pragma once

#include "KeyTable.h"

#include "DatabaseCore/StatementBase.h"
#include "Logger/Logger.h"

namespace Operator
{
	int GetEmptyKeys(std::vector<Operator::RegistrationKey>& keys);

	class EmptyKeysStatement
		:
		public Database::StatementBase
	{
	public:
		Operator::RegistrationKey key;

		int raw_execute(
			Database::SQLiteDatabase* const database,
			Database::SQLiteStatement& statement) override
		{
			std::stringstream ss;

			ss << "SELECT ";

			ss << KeyTable::getTableDefinition()->columns[KeyTable::Column::Key];

			ss << " FROM ";
			ss << KeyTable::getTableDefinition()->name;
			ss << " WHERE player IS NULL;";

			const std::string command = ss.str();

			Log::Information(L"Executing " + carrtowstr(command.c_str()));

			return sqlite3_prepare_v2(
				database->getSQLite(),
				command.c_str(),
				(int)command.size(),
				&statement.statement,
				NULL);
		}

	private:
		bool apply(sqlite3_stmt* const statement) override
		{
			if (sqlite3_column_type(statement, 0) == SQLITE_NULL)
			{
				Log::Error(L"Got null in apply in emptykey statement");

				return false;
			}

			memcpy(key.content,
				sqlite3_column_blob(statement, 0),
				OPERATOR_KEY_SIZE);

			return true;
		}
	};
}
