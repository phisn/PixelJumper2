#pragma once

#include "OperatorDatabase.h"

#include "Common/OperatorDefinitions.h"
#include "NetCore/NetCore.h"
#include "ResourceCore/ClassicPlayerResource.h"
#include "ResourceCore/PlayerResource.h"

// databaseinterface does only contain
// complex functions
// simple creation or extraction is usally
// implemented alongside the tabledefinitions
namespace Operator
{
	Database::ConditionResult GetUserType(
		Operator::UserType& userType,
		const Operator::UserID userID);

	Database::ConditionResult RetrivePlayerResource(
		Resource::PlayerResource* resource,
		UserID userID);

	// unable to distinguish between player does not
	// exists or player does not have any entries
	// query existance manually for that information
	bool RetriveClassicPlayerResource(
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

	bool CreateNewKey(
		Operator::RegistrationKey* const key,
		const Resource::PlayerID playerID = 0,
		const std::string source = "");

	bool CreateThreat(
		Operator::UserID userID,
		sf::Uint64 threatID,
		std::wstring message,
		::Net::ThreatLevel level);
}
