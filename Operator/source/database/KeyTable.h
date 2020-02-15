#pragma once

#include <Client/source/logger/Logger.h>
#include <Client/source/resource/PlayerResource.h>
#include <Operator/source/database/TableBase.h>

namespace Database
{
	class KeyTable
		:
		public TableBase
	{
		static const TableDefinition definition;

	public:
		struct Column
		{
			enum
			{
				Key,
				Player,
				Source
			};
		};

		KeyTable()
			:
			TableBase(&definition)
		{
		}

		struct Primary
		{
			Operator::RegistrationKey key;

		} primary;

		struct Content
		{
			std::string source;

		} content;

		struct Foreign
		{
			Resource::PlayerID playerID = 0;

		} foreign;

		static inline const TableDefinition* getTableDefinition()
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
			case Column::Key:
				return sqlite3_bind_blob(
					statement,
					columnIndex,
					primary.key.content,
					OPERATOR_KEY_SIZE,
					SQLITE_STATIC);

			case Column::Player:
				if (foreign.playerID == NULL)
				{
					return sqlite3_bind_null(statement, columnIndex);
				}

				return sqlite3_bind_int64(
					statement,
					columnIndex,
					foreign.playerID);

			case Column::Source:
				return sqlite3_bind_text(
					statement,
					columnIndex,
					content.source.c_str(),
					content.source.size(),
					SQLITE_STATIC);

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
				switch (column)
				{
				case Column::Player:
					foreign.playerID = NULL;

					break;
				case Column::Source:
					content.source.clear();

					break;
				default:
					Log::Error(L"Invalid null type in adoptcolumnvalue",
						column, L"column",
						columnIndex, L"columnindex");

					return false;
				}

				return true;
			}

			switch (column)
			{
			case Column::Key:
				memcpy(primary.key.content,
					sqlite3_column_blob(statement, columnIndex),
					OPERATOR_KEY_SIZE);

				break;
			case Column::Player:
				foreign.playerID = sqlite3_column_int64(statement, columnIndex);

				break;
			case Column::Source:
			{
				content.source.clear();

				const int usernameLength = sqlite3_column_bytes(statement, columnIndex);
				content.source.reserve(usernameLength / 2);
				content.source.assign(
					(const char*)sqlite3_column_blob(statement, columnIndex)
				);

				break;
			}
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
