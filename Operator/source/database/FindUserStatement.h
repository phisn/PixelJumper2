#pragma once

#include <Client/source/logger/Logger.h>
#include <Client/source/resource/PlayerResource.h>

#include <Operator/source/database/StatementBase.h>
#include <Operator/source/database/UserTable.h>

#include <sstream>

namespace Database
{
	class FindUserStatement
		:
		public StatementBase
	{
	public:
		UserTable userTable;

		int execute(
			sqlite3* const database,
			sqlite3_stmt** const statement) override
		{
			std::stringstream ss;

			ss << "SELECT ";

			const TableBase::ColumnValuesContainer allContainer = userTable.getAllColumnValues();
			for (const TableBase::ColumnValuesPair& pair : allContainer)
			{
				ss << pair.first;

				if (pair != allContainer.back())
				{
					ss << ",";
				}
			}

			ss << " FROM ";
			ss << userTable.getTableName();
			ss << " ";
			ss << condition;
			ss << ";";

			const std::string command = ss.str();

			Log::Information(L"Executing " + carrtowstr(command.c_str()));

			return sqlite3_prepare_v2(
				database,
				command.c_str(),
				(int) command.size(),
				statement,
				NULL);
		}

		void setConditionUsername(const std::string username)
		{
			std::stringstream ss;

			ss << "WHERE username='";
			ss << username;
			ss << '\'';

			condition = ss.str();
		}

		void setConditionToken(const char token[OPERATOR_HASH_SIZE])
		{
			std::stringstream ss;

			ss << "WHERE token='";
			ss << carrtohexstr((const unsigned char*) token, OPERATOR_HASH_SIZE);
			ss << '\'';

			condition = ss.str();
		}

	private:
		void apply(sqlite3_stmt* const statement) override
		{
			userTable.apply(statement);
		}

		std::string condition;
	};
}
