#pragma once

#include <Client/source/resource/PlayerResource.h>
#include <Operator/source/database/TableBase.h>

namespace Database
{
	class KeyTable
		:
		public TableBase
	{
	public:
		struct Primary
		{
			char key[15];

		} primary;

		struct Foreign
		{
			Resource::PlayerID playerID;

		} foreign;

		void apply(sqlite3_stmt* const statement) override
		{
			const char* rawKey = (const char*) sqlite3_column_blob(statement, 0);

			for (int i = 0; i < 18; ++i)
				if ((i % 6) != 5)
				{
					primary.key[i - i / 6] = rawKey[i];
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
					key += primary.key[column * 5 + i];
				}

				if (++column == 3)
				{
					break;
				}

				key += '-';
			}

			return key;
		}

	private:
		constexpr static const char* ColumnNames[] =
		{
			"key",
			"player"
		};

		constexpr static char CreateTable[] =
			R"__(
			CREATE TABLE "keys" (
				"key" BLOB NOT NULL,
				"player" INTEGER,
				PRIMARY KEY("key"),
				FOREIGN KEY("player") REFERENCES "players"("id") ON DELETE SET NULL
			);)__";

		const ColumnValuesContainer getAllColumnValues() override
		{
			ColumnValuesContainer container;

			container.emplace_back(ColumnNames[0], keyAsString());
			container.emplace_back(ColumnNames[1], foreign.playerID);

			return container;
		}

		const ColumnValuesContainer getPrimaryKeyColumnValues() override
		{
			ColumnValuesContainer container;

			container.emplace_back(ColumnNames[0], keyAsString());

			return container;
		}
	};
}
