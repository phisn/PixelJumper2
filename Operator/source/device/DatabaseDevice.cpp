#include "DatabaseDevice.h"

#include <Client/source/logger/Logger.h>

#include <Operator/source/database/PlayerTable.h>
#include <iostream>

namespace
{
	sqlite3* database;
}

namespace Device::Database
{
	bool VerifyDatabase();
	bool Initialize()
	{
		int result = sqlite3_open("pj2.db", &database);

		if (result)
		{
			 Log::Error(
				 std::wstring(L"Failed to open database (") +
				 (const wchar_t*) sqlite3_errmsg16(database) +
				L")",
				 result, L"result");

			 return false;
		}

		::Database::PlayerTable player;

		player.primary.id = 2657;

		sqlite3_stmt* statement;
		result = player.extract(database, &statement);

		if (result != SQLITE_OK)
		{
			Log::Error(L"Failed to extract", result, L"result");
			return false;
		}

		if (result = sqlite3_step(statement); result == SQLITE_ROW)
		{
			player.apply(statement);
			
			std::cout << "player: " << player.primary.id << std::endl;
			std::cout << "password: ";

			for (int i = 0; i < 20; ++i)
			{
				std::cout << std::hex << (int) player.content.password[i];
			}

			std::cout << std::endl << "salt: ";

			for (int i = 0; i < 16; ++i)
			{
				std::cout << std::hex << (int) player.content.salt[i];
			}

			std::cout << std::endl;
		}
		else
		{
			Log::Error(L"failed to step", result, L"result");
			return false;
		}

		return true;

		// return VerifyDatabase();
	}

	bool VerifyDatabase()
	{
		sqlite3_stmt* statement;
		int result = sqlite3_prepare_v2(database,
		R"__(CREATE TABLE 'players' (
			'id' INT NOT NULL,
			'salt' VARBINARY(16) NULL,
			'password' VARBINARY(20) NULL,
			PRIMARY KEY('id')))__",
			-1,
			&statement,
			NULL);

		if (result)
		{
			Log::Error(std::wstring(L"Failed to create statement (") +
				(const wchar_t*)sqlite3_errmsg16(database) +
				L")",
				result, L"result");

			return false;
		}

		result = sqlite3_step(statement);

		if (result != SQLITE_DONE)
		{
			Log::Error(L"Failed to run statement",
				result, L"result");

			return false;
		}

		result = sqlite3_finalize(statement);

		if (result)
		{
			Log::Error(L"Failed to finalize statement",
				result, L"result");

			return false;
		}

		return true;
	}

	void Unintialize()
	{
		sqlite3_close(database);
	}
}
