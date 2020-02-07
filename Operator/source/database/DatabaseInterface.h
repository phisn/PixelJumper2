#pragma once

#include <Client/source/resource/PlayerResource.h>

#include <Operator/source/database/TableBase.h>
#include <Operator/source/device/DatabaseDevice.h>

namespace Database::Interface
{
	Device::Database::ExtractionResult GetPlayerSalt(char salt[16], const Resource::PlayerID player);
	Device::Database::ExtractionResult GetPlayerHash(char hash[20], const Resource::PlayerID player);

	// call with null to playerid
	// to check if username exists
	Device::Database::ExtractionResult GetPlayerID(
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
