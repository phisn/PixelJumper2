#include "UnlockedWorldsTable.h"

const Database::TableDefinition Operator::UnlockedWorldsTable::definition
{
	"unlockedworlds",
	"pid",
	"wid"
};

namespace Operator
{
	bool UnlockWorld(UserID userID, Resource::WorldID worldID)
	{
		Log::Section section{ L"unlocking world",
				userID, L"userID",
				worldID, L"worldID" };

		UnlockedWorldsTable table;

		table.primary.userID = userID;
		table.primary.worldID = worldID;

		Database::ConditionResult result = table.exists(
		{
			UnlockedWorldsTable::Column::PlayerID,
			UnlockedWorldsTable::Column::WorldID
		});

		switch (result)
		{
		case Database::ConditionResult::Error:
			Log::Error(L"failed to identify weather a world is unlocked",
				userID, L"userID",
				worldID, L"worldID");

			return false;
		case Database::ConditionResult::Found:
			Log::Warning(L"world was already unlocked");

			return true;
		}

		if (!table.create())
		{
			Log::Error(L"failed to create unlock entry in database");

			return false;
		}

		return true;
	}
}
