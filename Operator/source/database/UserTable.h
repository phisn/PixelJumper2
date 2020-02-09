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
	public:
		UserTable()
			:
			TableBase("users")
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

		void apply(sqlite3_stmt* const statement) override
		{
			primary.id = sqlite3_column_int64(statement, 0);

			content.username.clear();
			const int usernameLength = sqlite3_column_bytes(statement, 1);
			content.username.reserve(usernameLength / 2);
			content.username.assign((const char*) sqlite3_column_blob(statement, 1));

			unsigned short* hex = (unsigned short*) sqlite3_column_blob(statement, 2);
			for (int i = 0; i < 20; ++i)
			{
				content.hash[i] = hextobyte(hex[i]);
			}

			hex = (unsigned short*) sqlite3_column_blob(statement, 3);
			for (int i = 0; i < 16; ++i)
			{
				content.salt[i] = hextobyte(hex[i]);
			}

			if (sqlite3_column_type(statement, 0) != SQLITE_NULL)
			{
				hex = (unsigned short*) sqlite3_column_blob(statement, 4);
			
				for (int i = 0; i < 16; ++i)
				{
					content.token[i] = hextobyte(hex[i]);
				}
			}
		}

		const ColumnValuesContainer getAllColumnValues() override
		{
			ColumnValuesContainer container;

			container.emplace_back(ColumnNames[0], std::to_string(primary.id));
			container.emplace_back(ColumnNames[1], '\'' + content.username + '\'');
			container.emplace_back(ColumnNames[2], '\'' + carrtohexstr(content.hash, OPERATOR_HASH_SIZE) + '\'');
			container.emplace_back(ColumnNames[3], '\'' + carrtohexstr(content.salt, OPERATOR_SALT_SIZE) + '\'');
			container.emplace_back(ColumnNames[4], '\'' + carrtohexstr(content.token, OPERATOR_HASH_SIZE) + '\'');


			return container;
		}

		const ColumnValuesContainer getPrimaryKeyColumnValues() override
		{
			ColumnValuesContainer container;

			container.emplace_back(ColumnNames[0], std::to_string(primary.id));

			return container;
		}

	private:
		constexpr static const char* ColumnNames[] =
		{
			"id",
			"username",
			"hash",
			"salt",
			"token"
		};
	};
}
