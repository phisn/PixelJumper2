#pragma once

#include "DatabaseCore.h"

namespace Database
{
	class Transaction
	{
	public:
		bool begin(SQLiteDatabase* database)
		{
			this->database = database;

			if (!Statement(database, "BEGIN TRANSACTION").execute())
			{
				finished = true;
				return false;
			}

			return true;
		}

		~Transaction()
		{
			if (database && !finished)
			{
				// result does not matter
				rollback();
			}
		}

		bool commit()
		{
			finished = true;
			return Statement(database, "COMMIT").execute();
		}

		bool rollback()
		{
			finished = true;
			return Statement(database, "ROLLBACK").execute();
		}

	private:
		SQLiteDatabase* database = NULL;
		bool finished = false;
	};
}
