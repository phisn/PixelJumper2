#pragma once

#include <sqlite3.h>

namespace Database
{
	class StatementBase
	{
	public:
		virtual int execute(
			sqlite3* const database, 
			sqlite3_stmt** const statement) = 0;

		int next(sqlite3_stmt* const statement)
		{
			const int result = sqlite3_step(statement);

			if (result == SQLITE_ROW)
			{
				if (!apply(statement))
				{
					return SQLITE_ERROR;
				}
			}
			else
			{
				const int finalize_result = sqlite3_finalize(statement);

				if (finalize_result != SQLITE_OK)
				{
					return finalize_result;
				}
			}

			return result;
		}

	protected:
		virtual bool apply(sqlite3_stmt* const statement) = 0;
	};
}
