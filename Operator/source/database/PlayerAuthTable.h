#pragma once

#include <Client/source/logger/Logger.h>
#include <Client/source/resource/PlayerResource.h>

#include <Operator/source/database/TableBase.h>

#include <SFML/Main.hpp>

namespace Database
{
	// to some virtual splitting
	// split the big table players in
	// many classes to support easy acess
	// -> authplayer, contentplayer ...
	class PlayerTable
		:
		public TableBase
	{
	public:
		PlayerTable()
			:
			TableBase("players")
		{
		}

		struct Primary
		{
			Resource::PlayerID id;

		} primary;

		struct Content
		{
			std::string username;

			unsigned char password[20];
			unsigned char salt[16];

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
				content.password[i] = hextobyte(hex[i]);
			}

			hex = (unsigned short*) sqlite3_column_blob(statement, 3);
			for (int i = 0; i < 16; ++i)
			{
				content.salt[i] = hextobyte(hex[i]);
			}
		}

		const ColumnValuesContainer getAllColumnValues() override
		{
			ColumnValuesContainer container;

			container.emplace_back(ColumnNames[0], std::to_string(primary.id));
			container.emplace_back(ColumnNames[1], content.username);
			container.emplace_back(ColumnNames[2], '\'' + carrtohexstr(content.password, 20) + '\'');
			container.emplace_back(ColumnNames[3], '\'' + carrtohexstr(content.salt, 16) + '\'');

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
			"password",
			"salt"
		};
	};
}
