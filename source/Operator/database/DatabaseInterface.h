#pragma once

#include "OperatorDatabase.h"

#include "Common/OperatorDefinitions.h"
#include "ResourceCore/ClassicPlayerResource.h"
#include "ResourceCore/PlayerResource.h"

namespace Operator
{
	struct UserAuthentication
	{
		UserID userID;

		char hash[OPERATOR_HASH_SIZE];
		char salt[OPERATOR_SALT_SIZE];
	};
}

namespace Operator::DatabaseInterface
{
	// token should not be sent to a player and
	// only used in hash. to send a player a token
	// a new one has to be created
	Database::ConditionResult GetPlayerToken(
		Operator::AuthenticationToken& token,
		const Operator::UserID userID);
	Database::ConditionResult GetPlayerAuth(
		UserAuthentication& authentication,
		const std::string username);

	Database::ConditionResult GetUserType(
		Operator::UserType& userType,
		const Operator::UserID userID);

	Database::ConditionResult FindUserID(
		Operator::UserID* const userID,
		const std::string username);
	Database::ConditionResult FindUserID(
		Operator::UserID* const userID,
		const char token[OPERATOR_HASH_SIZE]);

	Database::ConditionResult RetrivePlayerResource(
		Resource::PlayerResource* resource,
		UserID userID);
	Database::ConditionResult RetriveClassicPlayerResource(
		Resource::ClassicPlayerResource* resource,
		UserID userID);

	enum class CreatePlayerResult
	{
		Success,
		UsernameUsed,
		KeyUsed,
		KeyNotFound,
		Error,
	};

	// has to be inside a transaction to prevent
	// failure of following statements
	CreatePlayerResult CreateNewPlayer(
		Operator::UserID* const playerID,
		char token[OPERATOR_HASH_SIZE],
		const char salt[OPERATOR_SALT_SIZE],
		const char hash[OPERATOR_HASH_SIZE],
		const std::string username,
		const Operator::RegistrationKey key);

	bool CreatePlayerToken(
		char token[OPERATOR_HASH_SIZE],
		const Operator::UserID user);

	bool GetEmptyKeys(std::vector<Operator::RegistrationKey>& keys);
	bool CreateNewKey(
		Operator::RegistrationKey* const key,
		const Resource::PlayerID playerID = 0,
		const std::string source = "");
}
