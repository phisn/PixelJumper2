#pragma once

#include "Common/OperatorDefinitions.h"
#include "DatabaseCore/TableBase.h"
#include "Logger/Logger.h"
#include "OperatorDatabase.h"
#include "ResourceCore/PlayerResource.h"

namespace Operator
{
	bool UnlockRepresentation(UserID userID, Resource::RepresentationID representationID);

	class UnlockedRepresentationsTable
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
				RepresentationID
			};
		};

		UnlockedRepresentationsTable()
			:
			TableBase(&definition, Operator::OperatorDatabase::GetSQLiteDatabase())
		{
		}

		struct Primary
		{
			Operator::UserID userID;
			Resource::RepresentationID representationID;

		} primary;

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
					primary.userID);

			case Column::RepresentationID:
				return sqlite3_bind_int64(
					statement,
					columnIndex,
					primary.representationID);

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
				primary.userID = sqlite3_column_int64(statement, columnIndex);

				break;
			case Column::RepresentationID:
				primary.representationID = sqlite3_column_int64(statement, columnIndex);

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
