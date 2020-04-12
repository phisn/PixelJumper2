#pragma once

#include "Common/Common.h"
#include "DatabaseCore/TableBase.h"
#include "Logger/Logger.h"
#include "NetCore/NetCore.h"
#include "OperatorDatabase.h"
#include "ResourceCore/PlayerResource.h"

#include <SFML/Main.hpp>

namespace Operator
{
	class ThreatTable
		:
		public Database::TableBase
	{
		static const Database::TableDefinition definition;

	public:
		struct Column
		{
			enum
			{
				PlayerID,
				ThreatID,
				Message,
				Level
			};
		};

		ThreatTable()
			:
			TableBase(&definition, Operator::OperatorDatabase::GetSQLiteDatabase())
		{
		}

		struct Content
		{
			Operator::UserID playerID;
			sf::Uint64 threatID;
			Net::ThreatLevel level;
			std::wstring message;

		} content;

		static inline const Database::TableDefinition* getTableDefinition()
		{
			return &definition;
		}

	private:
		int bindColumnValue(
			sqlite3_stmt* const statement,
			const int column,
			const int columnIndex) const override
		{
			switch (column)
			{
			case Column::PlayerID:
				return sqlite3_bind_int64(
					statement,
					columnIndex,
					content.playerID);

			case Column::ThreatID:
				return sqlite3_bind_int64(
					statement,
					columnIndex,
					content.threatID);

			case Column::Message:
				return sqlite3_bind_blob(
					statement,
					columnIndex,
					content.message.c_str(),
					content.message.size() * sizeof(decltype(content.message)::value_type),
					SQLITE_STATIC);

			case Column::Level:
				return sqlite3_bind_int64(
					statement,
					columnIndex,
					(sqlite3_int64) content.level);
			}

			Log::Error(L"Got invalid column in bindcolumn",
				column, L"column",
				columnIndex, L"index");

			return SQLITE_ERROR;
		}

		bool adoptColumnValue(
			sqlite3_stmt* const statement,
			const int column,
			const int columnIndex) override
		{
			if (sqlite3_column_type(statement, columnIndex) == SQLITE_NULL)
			{
				return false;
			}

			switch (column)
			{
			case Column::PlayerID:
				content.playerID = sqlite3_column_int64(statement, columnIndex);

				break;
			case Column::ThreatID:
				content.threatID = sqlite3_column_int64(statement, columnIndex);

				break;
			case Column::Message:
				content.message.clear();

				int messageLength = sqlite3_column_bytes(statement, columnIndex);
				content.message.reserve(messageLength / 2);
				content.message.assign((const wchar_t*) sqlite3_column_blob(statement, columnIndex));

				break;
			case Column::Level:
				content.level = (Net::ThreatLevel) sqlite3_column_int64(statement, columnIndex);

				break;
			default:
				Log::Error(L"Got invalid column in adoptcolumn",
					column, L"column",
					columnIndex, L"index");

				return false;
			}

			return true;
		}
	};
}
