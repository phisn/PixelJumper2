#pragma once

#include <Client/source/logger/Logger.h>
#include <Client/source/resource/PlayerResource.h>

#include <Operator/source/database/StatementBase.h>

#include <sstream>

namespace Database
{
	class FindPlayerStatement
		:
		public StatementBase
	{
	public:
		struct Input
		{
			std::string username;

		} input;

		struct Output
		{
			Resource::PlayerID playerID;

		} output;

		int execute(
			sqlite3* const database,
			sqlite3_stmt** const statement) override
		{
			std::stringstream ss;

			ss << "SELECT id FROM players WHERE username='";
			ss << input.username;
			ss << '\'';

			const std::string command = ss.str();

			Log::Information(L"Executing " + carrtowstr(command.c_str()));

			return sqlite3_prepare_v2(
				database,
				command.c_str(),
				(int) command.size(),
				statement,
				NULL);
		}

	private:
		void apply(sqlite3_stmt* const statement) override
		{
			output.playerID = sqlite3_column_int64(statement, 0);
		}
	};
}
