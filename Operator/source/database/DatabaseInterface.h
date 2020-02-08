#pragma once

#include <Client/source/resource/PlayerResource.h>

#include <Operator/source/Common.h>

#include <Operator/source/database/TableBase.h>
#include <Operator/source/device/DatabaseDevice.h>

namespace Database::Interface
{
	Device::Database::ExtractionResult GetPlayerAuth(
		char hash[OPERATOR_HASH_SIZE],
		char salt[OPERATOR_SALT_SIZE],
		const Resource::PlayerID player);

	// call with null to playerid
	// to check if username exists
	Device::Database::ExtractionResult GetPlayerInfo(
		char hash[OPERATOR_HASH_SIZE],
		char salt[OPERATOR_SALT_SIZE],
		Resource::PlayerID* const playerID,
		const std::string username);

	enum class CreatePlayerResult
	{
		Success,
		UsernameUsed,
		KeyUsed,
		KeyNotFound,
		Error,
	};

	CreatePlayerResult CreateNewPlayer(
		Resource::PlayerID* playerID,
		char salt[16],
		char hash[20],
		const std::string username,
		const std::string key);

	bool GetEmptyKeys(std::vector<std::string>& keys);
	bool CreateNewKey(
		std::string* const key,
		const Resource::PlayerID playerID = 0,
		const std::string source = "");
}
