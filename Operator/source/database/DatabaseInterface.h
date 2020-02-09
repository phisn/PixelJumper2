#pragma once

#include <Client/source/resource/PlayerResource.h>

#include <Operator/source/Common.h>

#include <Operator/source/database/TableBase.h>
#include <Operator/source/device/DatabaseDevice.h>

namespace Database
{
	struct User
	{
		Operator::UserID userID;

		std::string username;

		char hash[OPERATOR_HASH_SIZE];
		char salt[OPERATOR_SALT_SIZE];
		char token[OPERATOR_HASH_SIZE];
	};

	namespace Interface
	{
		Device::Database::ExtractionResult GetPlayerAuth(
			char hash[OPERATOR_HASH_SIZE],
			char salt[OPERATOR_SALT_SIZE],
			const Resource::PlayerID player);

		// call with null
		// to check if username exists
		Device::Database::ExtractionResult GetUserInfo(
			User* const user,
			const std::string username);

		Device::Database::ExtractionResult GetUserInfo(
			User* const user,
			const Operator::UserID userID);

		Device::Database::ExtractionResult GetUserInfo(
			User* const user,
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
			char salt[OPERATOR_SALT_SIZE],
			char hash[OPERATOR_HASH_SIZE],
			const std::string username,
			const std::string key);

		bool CreatePlayerToken(
			char token[OPERATOR_HASH_SIZE],
			const Operator::UserID user);

		bool GetEmptyKeys(std::vector<std::string>& keys);
		bool CreateNewKey(
			std::string* const key,
			const Resource::PlayerID playerID = 0,
			const std::string source = "");
	}
}
