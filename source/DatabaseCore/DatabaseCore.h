#pragma once

#include "Logger/Logger.h"

#include <sqlite3.h>

#include <cassert>
#include <string>
#include <tuple>
#include <vector>

namespace Database
{
	typedef long long			SQLiteInt;
	typedef double				SQLiteReal;
	typedef std::string			SQLiteString;
	typedef std::vector<char>	SQLiteBlob;

	class SQLiteDatabase
	{
	public:
		sqlite3* raw()
		{
			return NULL;
		}
	};

	template <typename Arg, typename... Args>
	struct StatementVariadicInsert
	{
		template <typename TupleType, int Column = 0>
		static inline void Make(TupleType& tuple, sqlite3_stmt* statement)
		{
			StatementVariadicInsert<Arg>::Make<TupleType, Column>(tuple, statement);
			StatementVariadicInsert<Args...>::Make<TupleType, Column + 1>(tuple, statement);
		}
	};

	template <typename T>
	struct StatementVariadicInsert<std::optional<T>>
	{
		template <typename TupleType, int Column = 0>
		static inline void Make(TupleType& tuple, sqlite3_stmt* statement)
		{
			if (sqlite3_column_type(statement, Column) == SQLITE_NULL)
			{
				StatementVariadicInsert<T>::MakeNull<TupleType, Column>(tuple);
			}
			else
			{
				StatementVariadicInsert<T>::Make<TupleType, Column>(tuple, statement);
			}
		}
	};

	template <>
	struct StatementVariadicInsert<SQLiteInt>
	{
		template <typename TupleType, int Column = 0>
		static inline void Make(TupleType& tuple, sqlite3_stmt* statement)
		{
			assert(sqlite3_column_type(statement, Column) != SQLITE_NULL);
			std::get<Column>(tuple) = sqlite3_column_int64(statement, Column);
		}

		template <typename TupleType, int Column = 0>
		static inline void MakeNull(TupleType& tuple)
		{
			std::get<Column>(tuple) = 0;
		}
	};

	template <>
	struct StatementVariadicInsert<SQLiteReal>
	{
		template <typename TupleType, int Column = 0>
		static inline void Make(TupleType& tuple, sqlite3_stmt* statement)
		{
			assert(sqlite3_column_type(statement, Column) != SQLITE_NULL);
			std::get<Column>(tuple) = sqlite3_column_double(statement, Column);
		}

		template <typename TupleType, int Column = 0>
		static inline void MakeNull(TupleType& tuple)
		{
			std::get<Column>(tuple) = 0.0;
		}
	};

	template <>
	struct StatementVariadicInsert<SQLiteString>
	{
		template <typename TupleType, int Column = 0>
		static inline void Make(TupleType& tuple, sqlite3_stmt* statement)
		{
			assert(sqlite3_column_type(statement, Column) != SQLITE_NULL);
			std::get<0>(tuple) = std::string((const char*) sqlite3_column_text(statement, Column));
		}

		template <typename TupleType, int Column = 0>
		static inline void MakeNull(TupleType& tuple)
		{
			std::get<Column>(tuple).clear();
		}
	};

	template <>
	struct StatementVariadicInsert<SQLiteBlob>
	{
		template <typename TupleType, int Column = 0>
		static inline void Make(TupleType& tuple, sqlite3_stmt* statement)
		{
			assert(sqlite3_column_type(statement, Column) != SQLITE_NULL);
			const char* data = (const char*) sqlite3_column_blob(statement, Column);
			std::get<Column>(tuple).assign(data, data + sqlite3_column_bytes(statement, column));
		}

		template <typename TupleType, int Column = 0>
		static inline void MakeNull(TupleType& tuple)
		{
			std::get<Column>(tuple).clear();
		}
	};

	template <typename TupleType>
	struct StatementTupleInsert
	{
		static_assert(false, "tuple type expected in StatementTupleInsert");
	};

	template <typename... TupleArgs>
	struct StatementTupleInsert<std::tuple<TupleArgs...>>
		:
		public StatementVariadicInsert<TupleArgs...>
	{
	};

	template <typename StatementType>
	class StatementIterator
	{
	public:
		typedef StatementType::TupleType TupleType;

		typedef TupleType value_type;
		typedef std::ptrdiff_t difference_type;
		typedef TupleType* pointer;
		typedef TupleType& reference;
		typedef std::input_iterator_tag iterator_category;

		StatementIterator(StatementType* statement, bool isEnd)
			:
			statement(statement),
			isEnd(isEnd)
		{
		}

		const TupleType& operator*() const
		{
			return statement->getValue();
		}

		bool operator==(const StatementIterator& other) const
		{
			if (isEnd == other.isEnd)
			{
				return statement == other.statement;
			}
			else
			{
				return  statement == other.statement
					&& !statement->hasNext();
			}
		}

		bool operator!=(const StatementIterator& other) const
		{
			return !(*this == other);
		}

		StatementIterator& operator++()
		{
			statement->next();
			return *this;
		}

	private:
		bool isEnd;
		StatementType* statement;
	};

	template <typename Tuple>
	class Statement
	{
		// friend StatementIterator<Statement<Tuple>>;
		//

	public:
		typedef StatementIterator<Statement<Tuple>> Iterator;
		typedef Tuple TupleType;

		Statement(SQLiteDatabase* database, std::string query)
			:
			database(database)
		{
			ensureStatusCode(sqlite3_prepare_v2(
				database->raw(),
				query.c_str(),
				-1, &statement, NULL), L"failed to prepare statement");
			// next();
		}

		operator bool() const
		{
			return statement != NULL;
		}

		Iterator begin()
		{
			return Iterator{ this, false };
		}

		Iterator end()
		{
			return Iterator{ this, true };
		}

		bool next()
		{
			switch (int result = sqlite3_step(statement); result)
			{
			case SQLITE_ROW:
				StatementTupleInsert<TupleType>::Make(value, statement);

				return true;
			case SQLITE_DONE:
				finished = true;

				return false;
			default:
				ensureStatusCode(result, L"failed to step");

				return false;
			}
		}

		bool hasNext() const
		{
			return !finished;
		}

		const TupleType& getValue() const
		{
			return value;
		}

		bool reset()
		{
			return statement == NULL
				? false
				: ensureStatusCode(sqlite3_reset(statement), L"failed to reset statement");
		}

		bool clearBindings()
		{
			return statement == NULL
				? false
				: ensureStatusCode(sqlite3_clear_bindings(statement), L"failed to clear bindings");
		}

		sqlite3_stmt* raw() const
		{
			return statement;
		}

	private:
		TupleType value;
		bool finished = false;

		SQLiteDatabase* database;
		sqlite3_stmt* statement;

		bool ensureStatusCode(int statusCode, std::wstring info)
		{
			if (statusCode != SQLITE_OK)
			{
				// sqlite3_errmsg();
				Log::Error(info, statusCode, L"code");

				if (statusCode = sqlite3_finalize(statement); statusCode != SQLITE_OK)
					Log::Error(L"failed to finalize statement", statusCode, L"code");

				statement = NULL;

				return false;
			}

			return true;
		}

		inline void printStatusCodeError(int statusCode, std::wstring info)
		{
			Log::Error(info, statusCode, L"code");
		}
	};

	void _()
	{
		typedef std::tuple<int, int> ExtractTuple;
		Statement<ExtractTuple> tuple;

		typedef std::iterator_traits<ExtractTuple*> Test;
		Test t;
		Test::pointer p;
	}
}
