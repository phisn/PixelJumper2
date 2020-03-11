#pragma once

#include "DatabaseCore.h"

#include <sstream>
#include <vector>

namespace Database
{
	struct TableDefinition
	{
		template <typename... Args>
		TableDefinition(const std::string name, const Args... columns)
			:
			name(name),
			columns({ columns ... })
		{
		}

		std::string name;
		std::vector<std::string> columns;
	};

	//  think about adding attribute as a class
	
	class TableBase
	{
	public:
		typedef int ColumnIndex;
		typedef std::vector<ColumnIndex> Columns;

		TableBase(
			const TableDefinition* definition,
			SQLiteDatabase* const database)
			:
			definition(definition),
			database(database)
		{
		}

		ConditionResult remove(const Columns condition) 
		{
			return ConditionResult::Error;
		}

		ConditionResult exists(const Columns condition) const;
		
		bool create();		

		ConditionResult edit(
			const Columns info,
			const Columns condition);
		ConditionResult extract(
			const Columns info,
			const Columns condition);

		inline bool apply(
			sqlite3_stmt* const statement,
			const Columns columns)
		{
			for (int i = 0; i < columns.size(); ++i)
				if (!adoptColumnValue(statement, columns[i], i))
				{
					return false;
				}

			return true;
		}

		// only use if polymorphic traits are needed
		// use static getTableDefinition instead
		const TableDefinition* getPolyTableDefinition() const
		{
			return definition;
		}

	protected:
		virtual int bindColumnValue(
			sqlite3_stmt* const statement,
			const int column,
			const int columnIndex) const = 0;
		virtual bool adoptColumnValue(
			sqlite3_stmt* const statement,
			const int column,
			const int columnIndex) = 0;

	private:
		SQLiteDatabase* const database;
		const TableDefinition* definition;
	};
}
