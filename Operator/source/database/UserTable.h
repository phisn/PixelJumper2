#pragma once

#include <Client/source/logger/Logger.h>
#include <Client/source/resource/PlayerResource.h>

#include <Operator/source/Common.h>
#include <Operator/source/database/TableBase.h>

#include <SFML/Main.hpp>

namespace Database
{
	class UserTable
		:
		public TableBase
	{
		static const TableDefinition definition;

	public:
		struct Column
		{
			enum
			{
				PlayerID,
				Username,
				Hash,
				Salt,
				Token
			};
		};

		UserTable()
			:
			TableBase(&definition)
		{
		}

		struct Primary
		{
			Operator::UserID id;

		} primary;

		struct Content
		{
			std::string username;

			unsigned char hash[OPERATOR_HASH_SIZE];
			unsigned char salt[OPERATOR_SALT_SIZE];

			unsigned char token[OPERATOR_HASH_SIZE];

		} content;

		ConditionResult extractCommon(
			const Operator::UserID userID,
			const Columns info)
		{
			primary.id = userID;
			Columns condition{ Column::PlayerID };

			return extract(info, condition);
		}

		ConditionResult extractCommon(
			const std::string username,
			const Columns info)
		{
			content.username = username;
			Columns condition{ Column::Username };

			return extract(info, condition);
		}

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
			case Column::PlayerID:
				return sqlite3_bind_int64(
					statement, 
					columnIndex, 
					primary.id);

			case Column::Username:
				return sqlite3_bind_text(
					statement, 
					columnIndex, 
					content.username.c_str(),
					content.username.size(),
					SQLITE_STATIC);

			case Column::Hash:
				return sqlite3_bind_blob(
					statement,
					columnIndex,
					content.hash,
					OPERATOR_HASH_SIZE,
					SQLITE_STATIC);

			case Column::Salt:
				return sqlite3_bind_blob(
					statement,
					columnIndex,
					content.salt,
					OPERATOR_SALT_SIZE,
					SQLITE_STATIC);

			case Column::Token:
				bool null = true;

				for (int i = 0; i < OPERATOR_HASH_SIZE; ++i)
					if (content.token[i])
					{
						null = false;
						break;
					}

				if (null)
				{
					return sqlite3_bind_null(statement, columnIndex);
				}

				return sqlite3_bind_blob(
					statement,
					columnIndex,
					content.token,
					OPERATOR_HASH_SIZE,
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
				case Column::Token:
					for (int i = 0; i < OPERATOR_HASH_SIZE; ++i)
					{
						content.token[i] = 0;
					}

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
			case Column::PlayerID:
				primary.id = sqlite3_column_int64(statement, columnIndex);

				break;
			case Column::Username:
			{
				content.username.clear();

				const int usernameLength = sqlite3_column_bytes(statement, columnIndex);
				content.username.reserve(usernameLength / 2);
				content.username.assign(
					(const char*) sqlite3_column_blob(statement, columnIndex)
				);
			}
				break;
			case Column::Hash:
				memcpy(content.hash,
					sqlite3_column_blob(statement, columnIndex),
					OPERATOR_HASH_SIZE);

				break;
			case Column::Salt:
				memcpy(content.salt,
					sqlite3_column_blob(statement, columnIndex),
					OPERATOR_SALT_SIZE);

				break;
			case Column::Token:
				memcpy(content.token,
					sqlite3_column_blob(statement, columnIndex),
					OPERATOR_HASH_SIZE);

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
