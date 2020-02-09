#include "DatabaseInterface.h"

#include <Client/source/device/RandomDevice.h>

#include <Operator/source/database/EmptyKeyStatement.h>
#include <Operator/source/database/FindUserStatement.h>
#include <Operator/source/database/UserTable.h>

Device::Database::ExtractionResult FindUserWithStatement(
	Database::User* const user,
	Database::FindUserStatement& statement)
{
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

		if (user)
		{
			user->userID = statement.userTable.primary.id;
			user->username = statement.userTable.content.username;

			memcpy(user->hash, statement.userTable.content.hash, OPERATOR_HASH_SIZE);
			memcpy(user->salt, statement.userTable.content.salt, OPERATOR_SALT_SIZE);
			memcpy(user->token, statement.userTable.content.token, OPERATOR_HASH_SIZE);
		}

		return Device::Database::ExtractionResult::Found;

	default:
		Log::Error(L"Failed to next findplayer statement " + Device::Database::GenerateErrorMessage(),
			result, L"result");

		return Device::Database::ExtractionResult::Error;
	}
}

Device::Database::ExtractionResult Database::Interface::GetPlayerAuth(
	char hash[OPERATOR_HASH_SIZE], 
	char salt[OPERATOR_SALT_SIZE], 
	const Resource::PlayerID player)
{
	UserTable playerTable;
	playerTable.primary.id = player;

	const Device::Database::ExtractionResult result = Device::Database::Extract(&playerTable);

	if (result == Device::Database::ExtractionResult::Found)
	{
		memcpy(hash, playerTable.content.hash, 20);
		memcpy(salt, playerTable.content.salt, 16);
	}

	return result;
}

Device::Database::ExtractionResult Database::Interface::GetUserInfo(
	User* const user, 
	const Operator::UserID userID)
{
	UserTable playerTable;
	playerTable.primary.id = userID;

	const Device::Database::ExtractionResult result = Device::Database::Extract(&playerTable);

	if (result == Device::Database::ExtractionResult::Found && user)
	{
		user->userID = userID;
		user->username = std::move(playerTable.content.username);

		memcpy(user->hash, playerTable.content.hash, OPERATOR_HASH_SIZE);
		memcpy(user->salt, playerTable.content.salt, OPERATOR_SALT_SIZE);
		memcpy(user->token, playerTable.content.token, OPERATOR_HASH_SIZE);
	}

	return result;
}

Device::Database::ExtractionResult Database::Interface::GetUserInfo(
	User* const user, 
	const char token[OPERATOR_HASH_SIZE])
{
	FindUserStatement statement;
	statement.setConditionToken(token);

	return FindUserWithStatement(
		user,
		statement);
}

Device::Database::ExtractionResult Database::Interface::GetUserInfo(
	User* const user,
	const std::string username)
{
	FindUserStatement statement;
	statement.setConditionUsername(username);

	return FindUserWithStatement(
		user, 
		statement);
}

Database::Interface::CreatePlayerResult Database::Interface::CreateNewPlayer(
	Operator::UserID* resultPlayerID, 
	char salt[OPERATOR_SALT_SIZE], 
	char hash[OPERATOR_HASH_SIZE], 
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

	switch (GetUserInfo(NULL, username))
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

		UserTable table;
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
				Log::Error(L"Failed to generate userID for new player");

				return CreatePlayerResult::Error;
			}
		}
	}

	if (!Device::Database::BeginTransaction())
	{
		return CreatePlayerResult::Error;
	}

	UserTable playerTable;

	playerTable.primary.id = playerID;
	playerTable.content.username = username;

	memcpy(playerTable.content.hash, hash, 20);
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

bool Database::Interface::CreatePlayerToken(
	char token[OPERATOR_HASH_SIZE],
	const Operator::UserID userID)
{
	UserTable userTable;
	userTable.primary.id = userID;

	if (Device::Database::Extract(&userTable) != Device::Database::ExtractionResult::Found)
	{
		return false;
	}

	Device::Random::FillRandom(
		OPERATOR_HASH_SIZE, 
		(char*) userTable.content.token
	);

	if (token)
	{
		memcpy(token,
			userTable.content.token,
			OPERATOR_HASH_SIZE);
	}

	return Device::Database::Edit(&userTable);
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
