#pragma once
/*
#include <Client/source/resource/PlayerResource.h>
#include <Operator/source/database/TableBase.h>

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
			Resource::PlayerID id;

		} primary;

		struct Content
		{

		} content;

		void apply(sqlite3_stmt* const statement) override
		{
			primary.id = sqlite3_column_int64(statement, 0);
		}


	private:
		constexpr static const char* ColumnNames[] =
		{
			"id"
		};
	};
}
*/