#include "UserTable.h"

const Database::TableDefinition Operator::UserTable::definition
{
	"users",
	"id",
	"username",
	"hash",
	"salt",
	"token"
};

namespace Operator
{
	Database::ConditionResult GetPlayerToken(
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

	Database::ConditionResult GetPlayerAuth(
		UserAuthentication& authentication,
		std::string username)
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

	
	Database::ConditionResult FindUserID(
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

	Database::ConditionResult FindUserID(
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
}
