#pragma once

#include <Client/source/logger/Logger.h>
#include <Client/source/resource/PlayerResource.h>

#include <Operator/source/database/StatementBase.h>

#include <sstream>

namespace Database
{
	class FindPlayerStatement
		:
		public StatementBase
	{
	public:
		PlayerTable playerTable;

		int execute(
			sqlite3* const database,
			sqlite3_stmt** const statement) override
		{
			std::stringstream ss;

			ss << "SELECT ";

			const TableBase::ColumnValuesContainer allContainer = playerTable.getAllColumnValues();
			for (const TableBase::ColumnValuesPair& pair : allContainer)
			{
				ss << pair.first;

				if (pair != allContainer.back())
				{
					ss << ",";
				}
			}

			ss << " FROM players WHERE username='";
			ss << playerTable.content.username;
			ss << '\'';

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
		void apply(sqlite3_stmt* const statement) override
		{
			playerTable.apply(statement);
		}
	};
}
