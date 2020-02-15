#pragma once

#include <Client/source/resource/PlayerResource.h>

#include <Operator/source/Common.h>

#include <Operator/source/database/EmptyKeyStatement.h>
#include <Operator/source/database/TableBase.h>
#include <Operator/source/device/DatabaseDevice.h>

namespace Database
{
	struct UserAuthentication
	{
		Operator::UserID userID;

		char hash[OPERATOR_HASH_SIZE];
		char salt[OPERATOR_SALT_SIZE];
	};
}

namespace Database::Interface
{
	ConditionResult GetPlayerAuth(
		UserAuthentication& authentication,
		const std::string username);

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

	CreatePlayerResult CreateNewPlayer(
		Operator::UserID* playerID,
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
