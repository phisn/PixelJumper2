#include "DatabaseInterface.h"

#include <Client/source/device/RandomDevice.h>

#include <Operator/source/database/EmptyKeyStatement.h>
#include <Operator/source/database/FindPlayerStatement.h>
#include <Operator/source/database/PlayerAuthTable.h>

Device::Database::ExtractionResult Database::Interface::GetPlayerSalt(char salt[16], const Resource::PlayerID player)
{
	PlayerTable playerTable;
	playerTable.primary.id = player;

	const Device::Database::ExtractionResult result = Device::Database::Extract(&playerTable);

	if (result == Device::Database::ExtractionResult::Found)
	{
		memcpy(salt, playerTable.content.salt, 16);
	}

	return result;
}

Device::Database::ExtractionResult Database::Interface::GetPlayerHash(char hash[20], const Resource::PlayerID player)
{
	PlayerTable playerTable;
	playerTable.primary.id = player;

	const Device::Database::ExtractionResult result = Device::Database::Extract(&playerTable);

	if (result == Device::Database::ExtractionResult::Found)
	{
		memcpy(hash, playerTable.content.salt, 20);
	}

	return result;
}

Device::Database::ExtractionResult Database::Interface::GetPlayerID(
	Resource::PlayerID* const playerID,
	const std::string username)
{
	FindPlayerStatement statement;
	statement.input.username = username;

	sqlite3_stmt* raw_statement;
	int result = statement.execute(Device::Database::GetConnection(), &raw_statement);

	if (result != SQLITE_OK)
	{
		if (raw_statement)
			Device::Database::FinalizeStatement(raw_statement);

		return Device::Database::ExtractionResult::Error;
	}

	result = statement.next(raw_statement);

	switch (result)
	{
	case SQLITE_DONE:
		return Device::Database::ExtractionResult::NotFound;

	case SQLITE_ROW:
		if (!Device::Database::FinalizeStatement(raw_statement))
		{
			return Device::Database::ExtractionResult::Error;
		}

		if (playerID)
		{
			*playerID = statement.output.playerID;
		}

		return Device::Database::ExtractionResult::Found;

	default:
		Log::Error(L"Failed to next findplayer statement " + Device::Database::GenerateErrorMessage(),
			result, L"result");

		return Device::Database::ExtractionResult::Error;
	}
}

Database::Interface::CreatePlayerResult Database::Interface::CreateNewPlayer(
	Resource::PlayerID* resultPlayerID, 
	char salt[16], 
	char hash[20], 
	const std::string username,
	const std::string key)
{
	KeyTable keyTable;
	keyTable.keyFromString(key);

	switch (Device::Database::Extract(&keyTable))
	{
	case Device::Database::ExtractionResult::NotFound:
		return CreatePlayerResult::KeyNotFound;

	case Device::Database::ExtractionResult::Error:
		return CreatePlayerResult::Error;

	}

	if (keyTable.foreign.playerID != NULL)
	{
		return CreatePlayerResult::KeyUsed;
	}

	switch (GetPlayerID(NULL, username))
	{
	case Device::Database::ExtractionResult::Found:
		return CreatePlayerResult::UsernameUsed;

	case Device::Database::ExtractionResult::Error:
		return CreatePlayerResult::Error;

	}

	Resource::PlayerID playerID;
	
	while (true)
	{
		playerID = Device::Random::MakeRandom<sf::Uint64>();

		// theoretical max 18'446'744'073'709'551'615
		// database max 9'223'372'036'854'775'807
		// used max 999'999'999'999'999'999
		// used min 100'000'000'000'000'001

		if (playerID < 100'000'000'000'000'001 ||
			playerID > 999'999'999'999'999'999)
		{
			continue;
		}

		PlayerTable table;
		table.primary.id = playerID;

		Device::Database::ExtractionResult result = Device::Database::Extract(&table);

		if (result == Device::Database::ExtractionResult::NotFound)
		{
			break;
		}
		else
		{
			if (result == Device::Database::ExtractionResult::Error)
			{
				Log::Error(L"Failed to generate playerID for new player");

				return CreatePlayerResult::Error;
			}
		}
	}

	if (!Device::Database::BeginTransaction())
	{
		return CreatePlayerResult::Error;
	}

	PlayerTable playerTable;

	playerTable.primary.id = playerID;
	playerTable.content.username = username;

	memcpy(playerTable.content.password, hash, 20);
	memcpy(playerTable.content.salt, salt, 16);

	if (!Device::Database::Insert(&playerTable))
	{
		Device::Database::RollbackTransaction();
		return CreatePlayerResult::Error;
	}

	keyTable.foreign.playerID = playerID;

	if (!Device::Database::Edit(&keyTable))
	{
		Device::Database::RollbackTransaction();
		return CreatePlayerResult::Error;
	}

	if (Device::Database::EndTransaction())
	{
		if (resultPlayerID)
			*resultPlayerID = playerID;

		return CreatePlayerResult::Success;
	}
	else
	{
		return CreatePlayerResult::Error;
	}
}

bool Database::Interface::GetEmptyKeys(std::vector<std::string>& keys)
{
	Database::EmptyKeysStatement emptyKeys;

	sqlite3_stmt* statement = NULL;
	int result = emptyKeys.execute(Device::Database::GetConnection(), &statement);

	if (result != SQLITE_OK)
	{
		Log::Error(L"Failed to execute statement " + Device::Database::GenerateErrorMessage(),
			result, L"result");

		if (statement != NULL)
		{
			Device::Database::FinalizeStatement(statement);
		}

		return false;
	}

	while (true)
	{
		result = emptyKeys.next(statement);

		if (result != SQLITE_ROW)
		{
			break;
		}

		std::string key;
		key.reserve(17);

		for (int i = 0; i < 14; ++i)
		{
			key += emptyKeys.table.primary.key.content[i];

			if (i % 5 == 4)
				key += '-';
		}

		key += emptyKeys.table.primary.key.content[14];

		keys.push_back(key);
	}

	if (result != SQLITE_DONE)
	{
		Log::Error(L"Failed to process statement " + Device::Database::GenerateErrorMessage(),
			result, L"result");
	}

	return Device::Database::FinalizeStatement(statement);
}

bool Database::Interface::CreateNewKey(
	std::string* const key,
	const Resource::PlayerID playerID, 
	const std::string source)
{
	Database::KeyTable keyTable;

	keyTable.foreign.playerID = playerID;
	keyTable.content.source = source;

	Device::Random::FillRandom(15, keyTable.primary.key.content);

	for (int i = 0; i < 15; ++i)
	{
		if (keyTable.primary.key.content[i] < 0)
		{
			keyTable.primary.key.content[i] *= -1;
		}

		keyTable.primary.key.content[i] %= 26;
		keyTable.primary.key.content[i] += 'A';
	}

	sqlite3_stmt* statement;

	if (Device::Database::Insert(&keyTable))
	{
		if (key)
		{
			key->clear();
			key->reserve(17);

			for (int i = 0; i < 14; ++i)
			{
				*key += keyTable.primary.key.content[i];

				if (i % 5 == 4)
					*key += '-';
			}

			*key += keyTable.primary.key.content[14];
		}

		return true;
	}
	else
	{
		return false;
	}
}
