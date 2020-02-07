#pragma once

#include <Client/source/logger/Logger.h>
#include <Client/source/resource/PlayerResource.h>
#include <Operator/source/database/TableBase.h>

namespace Database
{
	// xxxxx-xxxxx-xxxxx
	struct RegistrationKey
	{
		char content[15];
	};

	class KeyTable
		:
		public TableBase
	{
	public:
		KeyTable()
			:
			TableBase("keys")
		{
		}

		struct Primary
		{
			RegistrationKey key;

		} primary;

		struct Content
		{
			std::string source;

		} content;

		struct Foreign
		{
			Resource::PlayerID playerID = 0;

		} foreign;

		void apply(sqlite3_stmt* const statement) override
		{
			keyFromString((const char*)sqlite3_column_blob(statement, 0));
			foreign.playerID = sqlite3_column_int64(statement, 1);
			content.source.assign(
				(const char*) sqlite3_column_text(statement, 2),
				sqlite3_column_bytes(statement, 2)
			);
		}

		std::string keyAsString() const
		{
			std::string key;
			key.reserve(17);

			int column = 0;

			while (true)
			{
				for (int i = 0; i < 5; ++i)
				{
					key += primary.key.content[column * 5 + i];
				}

				if (++column == 3)
				{
					break;
				}

				key += '-';
			}

			return key;
		}

		void keyFromString(const std::string key)
		{
			if (key.size() < 17)
			{
				Log::Error(L"Unable to convert key with invalid size",
					key.size(), L"size");

				return;
			}

			for (int i = 0; i < 17; ++i)
				if ((i % 6) != 5)
				{
					primary.key.content[i - i / 6] = key[i];
				}
		}

		const ColumnValuesContainer getAllColumnValues() override
		{
			ColumnValuesContainer container;

			container.emplace_back(ColumnNames[0], '\'' + keyAsString() + '\'');
			container.emplace_back(ColumnNames[1], foreign.playerID == 0
				? "NULL"
				: std::to_string(foreign.playerID));
			container.emplace_back(ColumnNames[2], content.source.length() == 0
				? "NULL"
				: '\'' + content.source + '\'');

			return container;
		}

		const ColumnValuesContainer getPrimaryKeyColumnValues() override
		{
			ColumnValuesContainer container;

			container.emplace_back(ColumnNames[0], '\'' + keyAsString() + '\'');

			return container;
		}

	private:
		constexpr static const char* ColumnNames[] =
		{
			"key",
			"player",
			"source"
		};
	};
}
