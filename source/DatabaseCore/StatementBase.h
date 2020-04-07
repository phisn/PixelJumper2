#pragma once

#include "DatabaseCore.h"

namespace Database
{
	class StatementBase
	{
	public:
		// statementbase is friend of sqlitestatement and
		// can access its database
		int execute(SQLiteStatement& statement)
		{
			return raw_execute(statement.parent, statement);
		}

		virtual int raw_execute(
			SQLiteDatabase* const database,
			SQLiteStatement& statement) = 0;

		// next does finalize the statement automatically
		int next(SQLiteStatement& statement)
		{
			const int result = sqlite3_step(statement.statement);

			if (result == SQLITE_ROW)
			{
				if (!apply(statement.statement))
					return SQLITE_ERROR;
			}
			else
			{
				statement.finalize();
			}

			return result;
		}

	protected:

		virtual bool apply(sqlite3_stmt* const statement) = 0;
	};
}
