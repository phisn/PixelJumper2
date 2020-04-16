#pragma once

#include "KeyTable.h"
#include "UserTable.h"

#include "DatabaseCore/StatementBase.h"
#include "Logger/Logger.h"

namespace Operator
{
	class UserTypeByID
		:
		public Database::StatementBase
	{
	public:
		Operator::UserType userType;
		Operator::UserID userID;

		int raw_execute(
			Database::SQLiteDatabase* const database,
			Database::SQLiteStatement& statement) override
		{
			std::stringstream ss;

			ss << "SELECT ";
			ss << KeyTable::getTableDefinition()->columns[KeyTable::Column::Type];
			ss << " FROM ";
			ss << UserTable::getTableDefinition()->name;

			ss << Database::CreateTableJoin<UserTable, KeyTable>(
				UserTable::Column::PlayerID,
				KeyTable::Column::Player);

			ss << " WHERE users.id=?";

			const std::string command = ss.str();

			Log::Information(L"Executing " + carrtowstr(command.c_str()));

			const int result = sqlite3_prepare_v2(
				database->getSQLite(),
				command.c_str(),
				(int)command.size(),
				&statement.statement,
				NULL);

			if (result != SQLITE_OK)
			{
				return result;
			}
			
			return sqlite3_bind_int64(statement.statement, 1, userID);
		}

	private:
		bool apply(sqlite3_stmt* const statement) override
		{
			if (sqlite3_column_type(statement, 0) == SQLITE_NULL)
			{
				Log::Error(L"Got null in apply in emptykey statement");
				return false;
			}

			userType = (Operator::UserType) sqlite3_column_int(statement, 0);

			return true;
		}
	};
}
