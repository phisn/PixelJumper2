#pragma once

#include "Logger/Logger.h"

#include <sqlite3.h>

#include <cassert>
#include <optional>
#include <string>
#include <tuple>
#include <vector>

namespace Database
{
	typedef int64_t						SQLiteInt;    // integral
	typedef double						SQLiteReal;   // floating point
	typedef std::string					SQLiteString; // char*
	typedef std::vector<unsigned char>	SQLiteBlob;   // unsigned char*

	class SQLiteDatabase
	{
	public:
		bool open(std::string filename)
		{
			Log::Section section(L"opening database", filename, L"filename");

			return ensureStatusCode(
				sqlite3_open(filename.c_str(), &database),
				L"failed to open database");
		}

		operator bool() const
		{
			return database != NULL;
		}

		inline void logStatusCodeError(int statusCode, std::wstring info)
		{
			Log::Error(info,
				getLastErrorMessage(), L"message",
				statusCode, L"code");
		}

		std::wstring getLastErrorMessage() const
		{
			return carrtowstr(sqlite3_errmsg(database));
		}

		sqlite3* raw()
		{
			return database;
		}

	private:
		sqlite3* database;

		bool ensureStatusCode(int statusCode, std::wstring info)
		{
			if (statusCode != SQLITE_OK)
			{
				logStatusCodeError(statusCode, info);

				if (database)
				{
					if (statusCode = sqlite3_close(database); statusCode != SQLITE_OK)
						logStatusCodeError(statusCode, L"failed to close database");

					database = NULL;
				}

				return false;
			}

			return true;
		}
	};

	/*class Transaction
	{
	public:
		Transaction(SQLiteDatabase* database)
			:
			database(database)
		{
		}

	private:
		SQLiteDatabase* database;
	};*/

	template <typename T, typename Lazy = void>
	struct StatementColumn
	{
	};

	template <typename T, typename Lazy>
	struct StatementColumn<std::optional<T>, Lazy>
	{
		template <typename TupleType, int Column = 0>
		static inline void Extract(TupleType& tuple, sqlite3_stmt* statement)
		{
			if (sqlite3_column_type(statement, Column) == SQLITE_NULL)
			{
				std::get<Column>(tuple).reset();
			}
			else
			{
				StatementColumn<T>::ExtractAs<TupleType, Column>(*std::get<Column>(tuple), statement);
			}
		}

		template <typename TupleType, int Column = 0>
		static inline int Bind(TupleType& tuple, sqlite3_stmt* statement)
		{
			if (std::get<Column>(tuple))
			{
				StatementColumn<T>::BindAs(*std::get<Column>(tuple), Column, statement);
			}
			else
			{
				return sqlite3_bind_null(statement, Column + 1);
			}
		}
	};

	// should not exist for Extract
	template <typename Lazy>
	struct StatementColumn<std::nullopt_t, Lazy>
	{
		template <typename TupleType, int Column = 0>
		static inline int Bind(TupleType& tuple, sqlite3_stmt* statement)
		{
			return sqlite3_bind_null(statement, Column + 1);
		}
	};

	template <typename T>
	struct StatementColumn<T, std::enable_if_t<std::is_integral_v<T>>>
	{
		template <typename TupleType, int Column = 0>
		static inline void Extract(TupleType& tuple, sqlite3_stmt* statement)
		{
			ExtractAs<Column>((SQLiteInt&) std::get<Column>(tuple), statement);
		}

		template <int Column = 0>
		static inline void ExtractAs(SQLiteInt& value, sqlite3_stmt* statement)
		{
			assert(sqlite3_column_type(statement, Column) != SQLITE_NULL);
			value = sqlite3_column_int64(statement, Column);
		}

		template <typename TupleType, int Column = 0>
		static inline int Bind(TupleType& tuple, sqlite3_stmt* statement)
		{
			return BindAs((SQLiteInt) std::get<Column>(tuple), Column, statement);
		}

		static inline int BindAs(SQLiteInt value, int Column, sqlite3_stmt* statement)
		{
			return sqlite3_bind_int64(statement, Column + 1, value);
		}
	};

	template <typename T>
	struct StatementColumn<T, std::enable_if_t<std::is_floating_point_v<T>>>
	{
		template <typename TupleType, int Column = 0>
		static inline void Extract(TupleType& tuple, sqlite3_stmt* statement)
		{
			ExtractAs<Column>((SQLiteReal&) std::get<Column>(tuple), statement);
		}

		template <int Column = 0>
		static inline void ExtractAs(SQLiteReal& value, sqlite3_stmt* statement)
		{
			assert(sqlite3_column_type(statement, Column) != SQLITE_NULL);
			value = sqlite3_column_double(statement, Column);
		}

		template <typename TupleType, int Column = 0>
		static inline int Bind(TupleType& tuple, sqlite3_stmt* statement)
		{
			return BindAs((SQLiteReal) std::get<Column>(tuple), Column, statement);
		}

		static inline int BindAs(SQLiteReal value, int Column, sqlite3_stmt* statement)
		{
			return sqlite3_bind_double(statement, Column + 1, value);
		}
	};

	template <typename Lazy>
	struct StatementColumn<SQLiteString, Lazy>
	{
		template <typename TupleType, int Column = 0>
		static inline void Extract(TupleType& tuple, sqlite3_stmt* statement)
		{
			ExtractAs<Column>(std::get<Column>(tuple), statement);
		}

		template <int Column = 0>
		static inline void ExtractAs(SQLiteString& value, sqlite3_stmt* statement)
		{
			assert(sqlite3_column_type(statement, Column) != SQLITE_NULL);
			value.assign((const SQLiteString::value_type*) sqlite3_column_text(statement, Column));
		}

		template <typename TupleType, int Column = 0>
		static inline int Bind(TupleType& tuple, sqlite3_stmt* statement)
		{
			return BindAs(std::get<Column>(tuple), Column, statement);
		}

		static inline int BindAs(SQLiteString& value, int Column, sqlite3_stmt* statement)
		{
			return sqlite3_bind_text(
				statement, Column + 1, value.c_str(),
				value.size() * sizeof(SQLiteString::value_type), 
				SQLITE_TRANSIENT);
		}
	};

	template <typename Lazy>
	struct StatementColumn<const SQLiteString::value_type*, Lazy>
	{
		template <typename TupleType, int Column = 0>
		static inline int Bind(TupleType& tuple, sqlite3_stmt* statement)
		{
			return BindAs(std::get<Column>(tuple), Column, statement);
		}

		static inline int BindAs(const SQLiteString::value_type* value, int Column, sqlite3_stmt* statement)
		{
			return sqlite3_bind_text(
				statement, 
				Column + 1, value,
				-1,
				SQLITE_TRANSIENT);
		}
	};

	// do not care whether the string is big enough
	// or not. user has to be prepared when char* is
	// used
	template <typename Lazy>
	struct StatementColumn<SQLiteString::value_type*, Lazy>
	{
		template <typename TupleType, int Column = 0>
		static inline void Extract(TupleType& tuple, sqlite3_stmt* statement)
		{
			ExtractAs<Column>(std::get<Column>(tuple), statement);
		}

		template <int Column = 0>
		static inline void ExtractAs(SQLiteString::value_type* value, sqlite3_stmt* statement)
		{
			assert(sqlite3_column_type(statement, Column) != SQLITE_NULL);
			memcpy(value, (const void*) sqlite3_column_text(statement, Column), sqlite3_column_bytes(statement, Column));
		}
	};

	template <typename Lazy>
	struct StatementColumn<SQLiteBlob, Lazy>
	{
		template <typename TupleType, int Column = 0>
		static inline void Extract(TupleType& tuple, sqlite3_stmt* statement)
		{
			ExtractAs<Column>(std::get<Column>(tuple), statement);
		}

		template <int Column = 0>
		static inline void ExtractAs(SQLiteBlob& value, sqlite3_stmt* statement)
		{
			assert(sqlite3_column_type(statement, Column) != SQLITE_NULL);
			const char* data = (const char*) sqlite3_column_blob(statement, Column);
			value.assign(data, data + sqlite3_column_bytes(statement, column));
		}

		template <typename TupleType, int Column = 0>
		static inline int Bind(TupleType& tuple, sqlite3_stmt* statement)
		{
			return BindAs(std::get<Column>(tuple), Column, statement);
		}

		static inline int BindAs(SQLiteBlob& value, int Column, sqlite3_stmt* statement)
		{
			return sqlite3_bind_blob(
				statement, Column + 1, 
				(const void*) &value[0],
				value.size(),
				SQLITE_TRANSIENT);
		}
	};

	template <typename Lazy>
	struct StatementColumn<const unsigned char*, Lazy>
	{
		template <typename TupleType, int Column = 0>
		static inline int Bind(TupleType& tuple, sqlite3_stmt* statement)
		{
			return BindAs(std::get<Column>(tuple), Column, statement);
		}

		static inline int BindAs(const unsigned char* value, int Column, sqlite3_stmt* statement)
		{
			return sqlite3_bind_blob(
				statement,
				Column + 1, (const void*) value,
				strlen((const char*) value),
				SQLITE_TRANSIENT);
		}
	};

	// do not care whether the string is big enough
	// or not. user has to be prepared when unsigned char* is
	// used
	template <typename Lazy>
	struct StatementColumn<unsigned char*, Lazy>
	{
		template <typename TupleType, int Column = 0>
		static inline void Extract(TupleType& tuple, sqlite3_stmt* statement)
		{
			ExtractAs<Column>(std::get<Column>(tuple), statement);
		}

		template <int Column = 0>
		static inline void ExtractAs(unsigned char* value, sqlite3_stmt* statement)
		{
			assert(sqlite3_column_type(statement, Column) != SQLITE_NULL);
			memcpy(value, sqlite3_column_blob(statement, Column), sqlite3_column_bytes(statement, Column));
		}
	};
	
	template <typename Character>
	struct StatementColumn<std::basic_string<Character>, 
		std::enable_if_t<!std::is_same_v<Character, SQLiteString>>>
	{
		typedef std::basic_string<Character> String;

		template <typename TupleType, int Column = 0>
		static inline void Extract(TupleType& tuple, sqlite3_stmt* statement)
		{
			ExtractAs<Column>(std::get<Column>(tuple), statement);
		}

		template <int Column = 0>
		static inline void ExtractAs(String& value, sqlite3_stmt* statement)
		{
			assert(sqlite3_column_type(statement, Column) != SQLITE_NULL);
			value.assign((const String::value_type*) sqlite3_column_blob(statement, Column));
		}

		template <typename TupleType, int Column = 0>
		static inline int Bind(TupleType& tuple, sqlite3_stmt* statement)
		{
			return BindAs(std::get<Column>(tuple), Column, statement);
		}

		static inline int BindAs(String& value, int Column, sqlite3_stmt* statement)
		{
			return sqlite3_bind_blob(
				statement, Column + 1, 
				(const void*) value.c_str(),
				value.size() * sizeof(String::value_type),
				SQLITE_TRANSIENT);
		}
	};

	template <typename Lazy>
	struct StatementColumn<const wchar_t*, Lazy>
	{
		template <typename TupleType, int Column = 0>
		static inline int Bind(TupleType& tuple, sqlite3_stmt* statement)
		{
			return BindAs(std::get<Column>(tuple), Column, statement);
		}

		static inline int BindAs(const wchar_t* value, int Column, sqlite3_stmt* statement)
		{
			return sqlite3_bind_blob(
				statement,
				Column + 1, (const void*) value,
				wcslen(value) * sizeof(wchar_t),
				SQLITE_TRANSIENT);
		}
	};

	template <typename Arg, typename... Args>
	struct StatementVariadicColumn
	{
		template <typename TupleType, int Column = 0>
		static inline void Extract(TupleType& tuple, sqlite3_stmt* statement)
		{
			StatementVariadicColumn<Arg>::Extract<TupleType, Column>(tuple, statement);
			StatementVariadicColumn<Args...>::Extract<TupleType, Column + 1>(tuple, statement);
		}

		template <typename TupleType, int Column = 0>
		static inline int Bind(TupleType& tuple, sqlite3_stmt* statement)
		{
			int result = StatementVariadicColumn<Arg>::Bind<TupleType, Column>(tuple, statement);

			if (result != SQLITE_OK)
			{
				return result;
			}

			return StatementVariadicColumn<Args...>::Bind<TupleType, Column + 1>(tuple, statement);
		}
	};

	template <typename Arg>
	struct StatementVariadicColumn<Arg>
	{
		template <typename TupleType, int Column = 0>
		static inline void Extract(TupleType& tuple, sqlite3_stmt* statement)
		{
			StatementColumn<Arg>::Extract<TupleType, Column>(tuple, statement);
		}

		template <typename TupleType, int Column = 0>
		static inline int Bind(TupleType& tuple, sqlite3_stmt* statement)
		{
			return StatementColumn<Arg>::Bind<TupleType, Column>(tuple, statement);
		}
	};

	template <typename TupleType>
	struct StatementTupleColumn
	{
	};

	template <typename... TupleArgs>
	struct StatementTupleColumn<std::tuple<TupleArgs...>>
		:
		public StatementVariadicColumn<TupleArgs...>
	{
	};

	template <typename StatementType>
	class StatementIterator
	{
	public:
		typedef typename StatementType::TupleType TupleType;

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
			if (!isEnd && !statement->next())
			{
				isEnd = true;
			}

			return *this;
		}

	private:
		bool isEnd;
		StatementType* statement;
	};

	template <typename Tuple = std::tuple<>>
	class Statement
	{
	public:
		typedef StatementIterator<Statement<Tuple>> Iterator;
		typedef Tuple TupleType;

		template <typename... Args>
		Statement(SQLiteDatabase* database, std::string query, Args... bindArgs)
			:
			Statement(database, query, std::make_tuple(bindArgs...))
		{
		}

		template <typename... Args>
		Statement(SQLiteDatabase* database, std::string query, std::tuple<Args...> bindTuple)
			:
			Statement(database, query)
		{
			bind(bindTuple);
		}

		template <typename... Args>
		Statement(SQLiteDatabase* database, std::string query)
			:
			database(database)
		{
			Log::Information(L"creating statement", 
				database, L"database", 
				query, L"query");
			ensureStatusCode(sqlite3_prepare_v2(
				database->raw(),
				query.c_str(),
				-1, &statement, NULL), L"failed to prepare statement");
		}

		~Statement()
		{
			if (statement)
			{
				int result = sqlite3_finalize(statement);

				if (result != SQLITE_OK)
					database->logStatusCodeError(result, L"failed to finalize statement");
			}
		}

		operator bool() const
		{
			return statement != NULL;
		}

		Iterator begin()
		{
			if (statement && !initiated)
			{
				next();
			}

			return Iterator{ this, false };
		}

		Iterator end()
		{
			return Iterator{ this, true };
		}

		// used for queries that do not
		// expect a result
		bool execute()
		{
			assert(!finished);

			if (statement == NULL || initiated)
				return false;

			if (int result = sqlite3_step(statement); result != SQLITE_DONE)
			{
				assert(result != SQLITE_ROW);
				ensureStatusCode(result, L"failed to step in execute");

				return false;
			}

			return true;
		}

		bool next()
		{
			assert(!finished);

			if (statement == NULL)
				return false;

			if (!initiated)
				initiated = true;

			switch (int result = sqlite3_step(statement); result)
			{
			case SQLITE_ROW:
				StatementTupleColumn<Tuple>::Extract(value, statement);

				return true;
			case SQLITE_DONE:
				finished = true;

				return false;
			default:
				ensureStatusCode(result, L"failed to step in next");

				return false;
			}
		}

		bool hasNext() const
		{
			return !finished && statement != NULL;
		}

		template <typename T>
		bool bind(int index, T& value)
		{
			return statement == NULL
				? false
				: ensureStatusCode(StatementColumn<T>::BindAs(value, index, statement),
					L"failed to bind column");
		}

		template <typename... Args>
		bool bind(Args... args)
		{
			return bind(std::make_tuple<Args...>(args...));
		}

		template <typename... Args>
		bool bind(std::tuple<Args...> tuple)
		{
			return statement == NULL
				? false
				: ensureStatusCode(StatementTupleColumn<std::tuple<Args...>>::Bind(tuple, statement),
					L"failed to bind column");
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

		const TupleType& getValue() const
		{
			return value;
		}

		sqlite3_stmt* raw() const
		{
			return statement;
		}

	private:
		TupleType value;

		bool initiated = false;
		bool finished = false;

		SQLiteDatabase* database;
		sqlite3_stmt* statement;

		bool ensureStatusCode(int statusCode, std::wstring info)
		{
			if (statusCode != SQLITE_OK)
			{
				database->logStatusCodeError(statusCode, info);

				if (statement)
				{
					if (statusCode = sqlite3_finalize(statement); statusCode != SQLITE_OK)
						database->logStatusCodeError(statusCode, L"failed to finalize statement");

					statement = NULL;
				}

				return false;
			}

			return true;
		}
	};

	inline bool QuickExecute(
		SQLiteDatabase* database, 
		std::string query)
	{
		Log::Information(L"quick execute",
			database, L"database",
			query, L"query");

		int result = sqlite3_exec(database->raw(), query.c_str(), 
			NULL, NULL, NULL);

		if (result != SQLITE_OK)
		{
			database->logStatusCodeError(result, L"failed to quick execute query");

			return false;
		}

		return true;
	}
}

using Database::SQLiteInt;
using Database::SQLiteReal;
using Database::SQLiteString;
using Database::SQLiteBlob;
