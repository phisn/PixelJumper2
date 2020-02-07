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
		KeyTable table;

		int execute(
			sqlite3* const database,
			sqlite3_stmt** const statement) override
		{
			std::stringstream ss;

			ss << "SELECT ";

			const TableBase::ColumnValuesContainer& container = table.getAllColumnValues();
			for (TableBase::ColumnValuesContainer::const_iterator iterator = container.cbegin();
				iterator != container.cend() - 1; ++iterator)
			{
				ss << iterator->first;
				ss << ",";
			}

			ss << container.back().first;

			ss << " FROM ";
			ss << table.getTableName();
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
		void apply(sqlite3_stmt* const statement) override
		{
			table.apply(statement);
		}
	};
}
