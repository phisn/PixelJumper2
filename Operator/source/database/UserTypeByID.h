#pragma once

#include <Client/source/logger/Logger.h>

#include <Operator/source/database/StatementBase.h>
#include <Operator/source/database/KeyTable.h>
#include <Operator/source/database/UserTable.h>

namespace Database
{
	class UserTypeByID
		:
		public StatementBase
	{
	public:
		Operator::UserType userType;
		Operator::UserID userID;

		int execute(
			sqlite3* const database,
			sqlite3_stmt** const statement) override
		{
			std::stringstream ss;

			ss << "SELECT ";
			ss << KeyTable::getTableDefinition()->columns[KeyTable::Column::Type];
			ss << " FROM ";
			ss << UserTable::getTableDefinition()->name;

			ss << CreateTableJoin<UserTable, KeyTable>(
				UserTable::Column::PlayerID,
				KeyTable::Column::Player);

			ss << " WHERE users.id=?";

			const std::string command = ss.str();

			Log::Information(L"Executing " + carrtowstr(command.c_str()));

			const int result = sqlite3_prepare_v2(
				database,
				command.c_str(),
				(int)command.size(),
				statement,
				NULL);

			if (result != SQLITE_OK)
			{
				return result;
			}
			
			return sqlite3_bind_int64(*statement, 1, userID);
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
