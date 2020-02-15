#include "DatabaseInterface.h"

#include <Client/source/device/RandomDevice.h>

#include <Operator/source/database/EmptyKeyStatement.h>
#include <Operator/source/database/UserTable.h>



Database::ConditionResult Database::Interface::GetPlayerAuth(
	UserAuthentication& authentication,
	const std::string username)
{
	UserTable userTable;
	const ConditionResult result = userTable.extractCommon(
		username,
		{
			UserTable::Column::PlayerID,
			UserTable::Column::Hash,
			UserTable::Column::Salt
		});

	if (result == ConditionResult::Found)
	{
		authentication.userID = userTable.primary.id;

		memcpy(authentication.hash, userTable.content.hash, OPERATOR_HASH_SIZE);
		memcpy(authentication.salt, userTable.content.salt, OPERATOR_SALT_SIZE);
	}

	return result;
}

Database::ConditionResult Database::Interface::FindUserID(
	Operator::UserID* const userID, 
	const std::string username)
{
	UserTable playerTable;

	const ConditionResult result = playerTable.extractCommon(
		username,
		{
			UserTable::Column::PlayerID
		});

	if (result == ConditionResult::Found && userID)
	{
		*userID = playerTable.primary.id;
	}

	return result;
}

Database::ConditionResult Database::Interface::FindUserID(
	Operator::UserID* const userID, 
	const char token[OPERATOR_HASH_SIZE])
{
	UserTable playerTable;

	memcpy(playerTable.content.token,
		token,
		OPERATOR_HASH_SIZE);

	const ConditionResult result = playerTable.extract(
		{
			UserTable::Column::PlayerID
		},
		{
			UserTable::Column::Token
		});

	if (result == ConditionResult::Found && userID)
	{
		*userID = playerTable.primary.id;
	}

	return result;
}

Database::Interface::CreatePlayerResult Database::Interface::CreateNewPlayer(
	Operator::UserID* resultPlayerID, 
	const char salt[OPERATOR_SALT_SIZE], 
	const char hash[OPERATOR_HASH_SIZE], 
	const std::string username,
	const Operator::RegistrationKey key)
{
	KeyTable keyTable;

	memcpy(keyTable.primary.key.content,
		key.content,
		OPERATOR_KEY_SIZE);

	ConditionResult result = keyTable.extract(
		{
			KeyTable::Column::Player
		},
		{
			KeyTable::Column::Key
		});

	switch (result)
	{
	case ConditionResult::NotFound:
		return CreatePlayerResult::KeyNotFound;

	case ConditionResult::Error:
		return CreatePlayerResult::Error;

	}

	if (keyTable.foreign.playerID != NULL)
	{
		return CreatePlayerResult::KeyUsed;
	}

	switch (FindUserID(NULL, username))
	{
	case ConditionResult::Found:
		return CreatePlayerResult::UsernameUsed;

	case ConditionResult::Error:
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

		result = table.exists({ UserTable::Column::PlayerID });

		if (result == ConditionResult::NotFound)
		{
			break;
		}
		else
		{
			if (result == ConditionResult::Error)
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

	memcpy(playerTable.content.hash, hash, OPERATOR_HASH_SIZE);
	memcpy(playerTable.content.salt, salt, OPERATOR_SALT_SIZE);

	if (!playerTable.create())
	{
		Device::Database::RollbackTransaction();
		return CreatePlayerResult::Error;
	}

	keyTable.foreign.playerID = playerID;

	if (keyTable.edit(
		{
			KeyTable::Column::Player
		},
		{
			KeyTable::Column::Key
		}) != ConditionResult::Found)
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
	
	ConditionResult result = userTable.extractCommon(userID,
		{
			UserTable::Column::Token
		});

	if (result != ConditionResult::Found)
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

	return userTable.edit(
		{
			UserTable::Column::Token
		},
		{
			UserTable::Column::PlayerID
		}) 
		== ConditionResult::Found;
}

bool Database::Interface::GetEmptyKeys(std::vector<Operator::RegistrationKey>& keys)
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

		keys.emplace_back();

		memcpy(
			keys.back().content,
			emptyKeys.key.content,
			OPERATOR_KEY_SIZE);
	}

	if (result != SQLITE_DONE)
	{
		Log::Error(L"Failed to process statement " + Device::Database::GenerateErrorMessage(),
			result, L"result");
	}

	return Device::Database::FinalizeStatement(statement);
}

bool Database::Interface::CreateNewKey(
	Operator::RegistrationKey* const key,
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

	if (keyTable.create())
	{
		if (key) 
			memcpy(key->content,
				keyTable.primary.key.content,
				OPERATOR_KEY_SIZE);

		return true;
	}
	else
	{
		return false;
	}
}
