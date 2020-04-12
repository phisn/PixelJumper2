#include "DatabaseInterface.h"

#include "Common/RandomModule.h"

#include "EmptyKeyStatement.h"
#include "ThreatTable.h"
#include "UserTable.h"
#include "UserTypeByID.h"

namespace Operator
{
	Database::ConditionResult DatabaseInterface::GetPlayerToken(
		Operator::AuthenticationToken& token,
		const Operator::UserID userID)
	{
		UserTable userTable;
		const Database::ConditionResult result = userTable.extractCommon(
			userID,
			{
				UserTable::Column::Token
			});

		if (result == Database::ConditionResult::Found)
		{
			memcpy(token.token, userTable.content.token, sizeof(token));
		}

		return result;
	}

	Database::ConditionResult DatabaseInterface::GetPlayerAuth(
		UserAuthentication& authentication,
		const std::string username)
	{
		UserTable userTable;
		const Database::ConditionResult result = userTable.extractCommon(
			username,
			{
				UserTable::Column::PlayerID,
				UserTable::Column::Hash,
				UserTable::Column::Salt
			});

		if (result == Database::ConditionResult::Found)
		{
			authentication.userID = userTable.primary.id;

			memcpy(authentication.hash, userTable.content.hash, OPERATOR_HASH_SIZE);
			memcpy(authentication.salt, userTable.content.salt, OPERATOR_SALT_SIZE);
		}

		return result;
	}

	Database::ConditionResult DatabaseInterface::GetUserType(
		Operator::UserType& userType,
		const Operator::UserID userID)
	{
		UserTypeByID utbID;
		utbID.userID = userID;

		Operator::OperatorDatabaseStatement statement;
		int result = utbID.execute(statement);

		if (result != SQLITE_OK)
		{
			Log::Error(L"Failed to execute statement " + OperatorDatabase::GetErrorMessage(),
				result, L"result");

			return Database::ConditionResult::Error;
		}

		result = utbID.next(statement);

		switch (result)
		{
		case SQLITE_DONE:
			return Database::ConditionResult::NotFound;

		case SQLITE_ROW:
			userType = utbID.userType;

			break;
		default:
			Log::Error(L"Failed to process statement " + OperatorDatabase::GetErrorMessage(),
				result, L"result");

			return Database::ConditionResult::Error;
		}

		return Database::ConditionResult::Found;
	}

	Database::ConditionResult DatabaseInterface::FindUserID(
		Operator::UserID* const userID,
		const std::string username)
	{
		UserTable playerTable;

		const Database::ConditionResult result = playerTable.extractCommon(
			username,
			{
				UserTable::Column::PlayerID
			});

		if (result == Database::ConditionResult::Found && userID)
		{
			*userID = playerTable.primary.id;
		}

		return result;
	}

	Database::ConditionResult DatabaseInterface::FindUserID(
		Operator::UserID* const userID,
		const char token[OPERATOR_HASH_SIZE])
	{
		UserTable playerTable;

		memcpy(playerTable.content.token,
			token,
			OPERATOR_HASH_SIZE);

		const Database::ConditionResult result = playerTable.extract(
			{
				UserTable::Column::PlayerID
			},
		{
			UserTable::Column::Token
		});

		if (result == Database::ConditionResult::Found && userID)
		{
			*userID = playerTable.primary.id;
		}

		return result;
	}

	Database::ConditionResult DatabaseInterface::RetrivePlayerResource(
		Resource::PlayerResource* resource, 
		UserID userID)
	{
	}

	Database::ConditionResult DatabaseInterface::RetriveClassicPlayerResource(
		Resource::ClassicPlayerResource* resource, 
		UserID userID)
	{
	}

	DatabaseInterface::CreatePlayerResult Operator::DatabaseInterface::CreateNewPlayer(
		Operator::UserID* const resultPlayerID,
		char token[OPERATOR_HASH_SIZE],
		const char salt[OPERATOR_SALT_SIZE],
		const char hash[OPERATOR_HASH_SIZE],
		const std::string username,
		const Operator::RegistrationKey key)
	{
		KeyTable keyTable;

		memcpy(keyTable.primary.key.content,
			key.content,
			OPERATOR_KEY_SIZE);

		Database::ConditionResult result = keyTable.extract(
			{
				KeyTable::Column::Player
			},
		{
			KeyTable::Column::Key
		});

		switch (result)
		{
		case Database::ConditionResult::NotFound:
			return CreatePlayerResult::KeyNotFound;

		case Database::ConditionResult::Error:
			return CreatePlayerResult::Error;

		}

		if (keyTable.foreign.playerID != NULL)
		{
			return CreatePlayerResult::KeyUsed;
		}

		switch (FindUserID(NULL, username))
		{
		case Database::ConditionResult::Found:
			return CreatePlayerResult::UsernameUsed;

		case Database::ConditionResult::Error:
			return CreatePlayerResult::Error;

		}

		Resource::PlayerID playerID;

		while (true)
		{
			playerID = Module::Random::MakeRandom<sf::Uint64>();

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

			if (result == Database::ConditionResult::NotFound)
			{
				break;
			}
			else
			{
				if (result == Database::ConditionResult::Error)
				{
					Log::Error(L"Failed to generate userID for new player");

					return CreatePlayerResult::Error;
				}
			}
		}

		if (!Operator::OperatorDatabase::GetSQLiteDatabase()->beginTransaction())
		{
			return CreatePlayerResult::Error;
		}

		UserTable playerTable;

		playerTable.primary.id = playerID;
		playerTable.content.username = username;

		memcpy(playerTable.content.hash, hash, OPERATOR_HASH_SIZE);
		memcpy(playerTable.content.salt, salt, OPERATOR_SALT_SIZE);

		Module::Random::FillRandom(
			OPERATOR_HASH_SIZE,
			(char*)playerTable.content.token);

		if (token)
		{
			memcpy(token,
				playerTable.content.token,
				OPERATOR_HASH_SIZE);
		}

		if (!playerTable.create())
		{
			Operator::OperatorDatabase::GetSQLiteDatabase()->rollbackTransaction();
			return CreatePlayerResult::Error;
		}

		keyTable.foreign.playerID = playerID;

		if (keyTable.edit(
			{
				KeyTable::Column::Player
			},
		{
			KeyTable::Column::Key
		}) != Database::ConditionResult::Found)
		{
			Operator::OperatorDatabase::GetSQLiteDatabase()->rollbackTransaction();
			return CreatePlayerResult::Error;
		}

		if (Operator::OperatorDatabase::GetSQLiteDatabase()->endTransaction())
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

	bool DatabaseInterface::CreatePlayerToken(
		char token[OPERATOR_HASH_SIZE],
		const Operator::UserID userID)
	{
		UserTable userTable;
		userTable.primary.id = userID;

		Module::Random::FillRandom(
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
			== Database::ConditionResult::Found;
	}

	bool DatabaseInterface::GetEmptyKeys(std::vector<Operator::RegistrationKey>& keys)
	{
		EmptyKeysStatement emptyKeys;

		OperatorDatabaseStatement statement;
		int result = emptyKeys.execute(statement);

		if (result != SQLITE_OK)
		{
			Log::Error(L"Failed to execute statement " + OperatorDatabase::GetErrorMessage(),
				result, L"result");

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
			Log::Error(L"Failed to process statement " + OperatorDatabase::GetErrorMessage(),
				result, L"result");
		}

		return result == SQLITE_DONE;
	}

	bool DatabaseInterface::CreateNewKey(
		Operator::RegistrationKey* const key,
		const Resource::PlayerID playerID,
		const std::string source)
	{
		KeyTable keyTable;

		keyTable.foreign.playerID = playerID;
		keyTable.content.source = source;

		Module::Random::FillRandom(15, keyTable.primary.key.content);

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

	bool DatabaseInterface::CreateThreat(
		Operator::UserID userID,
		sf::Uint64 threatID,
		std::wstring message,
		::Net::ThreatLevel level)
	{
		ThreatTable table;
		table.content.playerID = userID;
		table.content.threatID = threatID;
		table.content.message = message;
		table.content.level = level;
		return table.create();
	}
}
