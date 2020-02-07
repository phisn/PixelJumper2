#pragma once

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

		struct Foreign
		{
			Resource::PlayerID playerID = 0;

		} foreign;

		void apply(sqlite3_stmt* const statement) override
		{
			const char* rawKey = (const char*) sqlite3_column_blob(statement, 0);

			for (int i = 0; i < 18; ++i)
				if ((i % 6) != 5)
				{
					primary.key.content[i - i / 6] = rawKey[i];
				}

			foreign.playerID = sqlite3_column_int64(statement, 1);
		}

		std::string keyAsString() const
		{
			std::string key;
			key.reserve(18);

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

		const ColumnValuesContainer getAllColumnValues() override
		{
			ColumnValuesContainer container;

			container.emplace_back(ColumnNames[0], keyAsString());
			container.emplace_back(ColumnNames[1], foreign.playerID == 0
				? "NULL"
				: std::to_string(foreign.playerID));

			return container;
		}

		const ColumnValuesContainer getPrimaryKeyColumnValues() override
		{
			ColumnValuesContainer container;

			container.emplace_back(ColumnNames[0], keyAsString());

			return container;
		}

	private:
		constexpr static const char* ColumnNames[] =
		{
			"key",
			"player"
		};
	};
}
