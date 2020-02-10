#pragma once

#include <Operator/source/device/DatabaseDevice.h>

#include <sqlite3.h>

#include <sstream>
#include <string>
#include <vector>

namespace Database
{
	// need to rework tablebase because of sqlinjections
	// to prevent sqlinjections in sqlite you have to use
	// parameterized statements where these parameters 
	// can later be bound to as values with sepicific types
	// because we dont know the types anymore at statement
	// construction we have to rethink this whole structure.
	// a another important aspect is the efficiency with
	// larger tables and maybe the sepcialization of primary
	// keys. because we often dont need all information or
	// need it based on different conditions we have to
	// add a dynamic aspect to solve this problem. this would
	// also eliminate the problem of sqlbinding in
	// findstatements because they would be of no need
	// anymore

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

	enum class ConditionResult
	{
		Found,
		NotFound,
		Error
	};

	//  think about adding attribute as a class
	
	class TableBase
	{
	public:
		typedef int ColumnIndex;
		typedef std::vector<ColumnIndex> Columns;

		TableBase(const TableDefinition* definition)
			:
			definition(definition)
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
				if (!adoptColumnValue(statement, columns[i], i + 1))
				{
					return false;
				}
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

		const TableDefinition* definition;
	};
}
