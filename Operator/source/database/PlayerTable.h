#pragma once

#include <Operator/source/database/TableBase.h>

#include <SFML/Main.hpp>

namespace Database
{
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
			sf::Uint64 id;

		} primary;

		struct Content
		{
			unsigned char password[20];
			unsigned char salt[16];

		} content;

		void apply(sqlite3_stmt* const statement) override
		{
			primary.id = sqlite3_column_int64(statement, 0);

			unsigned short* hex = (unsigned short*) sqlite3_column_blob(statement, 1);
			for (int i = 0; i < 20; ++i)
			{
				content.password[i] = hextobyte(hex[i]);
			}

			hex = (unsigned short*) sqlite3_column_blob(statement, 2);
			for (int i = 0; i < 16; ++i)
			{
				content.salt[i] = hextobyte(hex[i]);
			}
		}

	private:
		constexpr static const char* ColumnNames[] =
		{
			"id",
			"password",
			"salt"
		};
		
		const ColumnValuesContainer getAllColumnValues() override
		{
			ColumnValuesContainer container;

			container.emplace_back(ColumnNames[0], std::to_string(primary.id));
			container.emplace_back(ColumnNames[1], '\'' + carrtohexstr(content.password, 20) + '\'');
			container.emplace_back(ColumnNames[2], '\'' + carrtohexstr(content.salt, 16) + '\'');
			
			return container;
		}

		const ColumnValuesContainer getPrimaryKeyColumnValues() override
		{
			ColumnValuesContainer container;

			container.emplace_back(ColumnNames[0], std::to_string(primary.id));

			return container;
		}
	};
}
