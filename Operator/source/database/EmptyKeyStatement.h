#pragma once

#include <Client/source/logger/Logger.h>

#include <Operator/source/database/StatementBase.h>
#include <Operator/source/database/KeyTable.h>

namespace Database
{
	class EmptyKeysStatement
		:
		public StatementBase
	{
	public:
		Operator::RegistrationKey key;

		int execute(
			sqlite3* const database,
			sqlite3_stmt** const statement) override
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
				database,
				command.c_str(),
				(int) command.size(),
				statement,
				NULL);
		}

	private:
		bool apply(sqlite3_stmt* const statement) override
		{
			if (sqlite3_column_type(statement, 1) == SQLITE_NULL)
			{
				Log::Error(L"Got null in apply in emptykey statement");

				return false;
			}

			memcpy(key.content,
				sqlite3_column_blob(statement, 1),
				OPERATOR_KEY_SIZE);

			return true;
		}
	};
}
