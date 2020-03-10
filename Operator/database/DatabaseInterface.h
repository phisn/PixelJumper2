#pragma once

#include "Resource/PlayerResource.h"

#include "Common/OperatorDefinitions.h"

#include <Operator/source/database/EmptyKeyStatement.h>
#include <Operator/source/database/TableBase.h>
#include <Operator/source/device/DatabaseDevice.h>

namespace Operator
{
	struct UserAuthentication
	{
		Operator::UserID userID;

		char hash[OPERATOR_HASH_SIZE];
		char salt[OPERATOR_SALT_SIZE];
	};
}

namespace Operator::DatabaseInterface
{
	// token should not be sent to a player and
	// only used in hash. to send a player a token
	// a new one has to be created
	ConditionResult GetPlayerToken(
		Operator::AuthenticationToken& token,
		const Operator::UserID userID);
	ConditionResult GetPlayerAuth(
		UserAuthentication& authentication,
		const std::string username);

	ConditionResult GetUserType(
		Operator::UserType& userType,
		const Operator::UserID userID);

	ConditionResult FindUserID(
		Operator::UserID* const userID,
		const std::string username);
	ConditionResult FindUserID(
		Operator::UserID* const userID,
		const char token[OPERATOR_HASH_SIZE]);

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
